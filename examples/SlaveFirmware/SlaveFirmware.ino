#include <Wire.h>
#include <WiredController_asukiaaa.h>

#ifndef BTN_TOP
#define BTN_TOP    4
#endif
#ifndef BTN_LEFT
#define BTN_LEFT   5
#endif
#ifndef BTN_RIGHT
#define BTN_RIGHT  6
#endif
#ifndef BTN_BOTTOM
#define BTN_BOTTOM 7
#endif
#define LED_1 9
#define LED_2 10
#define LED_3 0
#define LED_4 1
#define JOYSTICK_HORIZONTAL_MIDDLE A0
#define JOYSTICK_HORIZONTAL_UPPER A1
#define JOYSTICK_HORIZONTAL_REVERSE
#define JOYSTICK_VERTICAL_MIDDLE A2
#define JOYSTICK_VERTICAL_UPPER A3
#define JOYSTICK_VERITCAL_REVERSE
#define JOYSTICK_MAX_VALUE WIRED_CONTROLLER_ASUKIAAA_JOYSTICK_MAX_VALUE
#define BTN_JOY 2 // Disable this for atmega32u4
#define ADDR_JUMPER 8

#define ANALOG_MAX 1023

#define RESISTER_JOYSTICK 10000
#define RESISTER_UPPER_FAIL_SAFE 990

#define REGISTER_LEN 0x06

// #define DEBUG

uint8_t registers[REGISTER_LEN];
uint8_t registerIndex = 0;

bool pushed(uint8_t pin) {
  return digitalRead(pin) == LOW;
}

uint16_t getJoystickValue(uint8_t upperPin, uint8_t middlePin) {
  uint16_t u = analogRead(upperPin);
  if (u > ANALOG_MAX * 0.98) {
    // Detect as joystick is not connected so return middle value.
    return JOYSTICK_MAX_VALUE / 2;
  }
  uint32_t v = analogRead(middlePin);
  v = JOYSTICK_MAX_VALUE * v / ANALOG_MAX;
  v = v * (RESISTER_JOYSTICK + RESISTER_UPPER_FAIL_SAFE) / RESISTER_JOYSTICK;
  return min(v, JOYSTICK_MAX_VALUE);
}

int16_t getJoystickHorizontalValue() {
  int16_t v = getJoystickValue(JOYSTICK_HORIZONTAL_UPPER, JOYSTICK_HORIZONTAL_MIDDLE);
#ifdef JOYSTICK_HORIZONTAL_REVERSE
  return JOYSTICK_MAX_VALUE - v;
#else
  return v;
#endif
}

int16_t getJoystickVerticalValue() {
  int16_t v = getJoystickValue(JOYSTICK_VERTICAL_UPPER, JOYSTICK_VERTICAL_MIDDLE);
#ifdef JOYSTICK_VERITCAL_REVERSE
  return JOYSTICK_MAX_VALUE - v;
#else
  return v;
#endif
}

void updateLeds() {
  uint8_t val = registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS];
  digitalWrite(LED_1, ((val & 0b0001) != 0));
  digitalWrite(LED_2, ((val & 0b0010) != 0));
  digitalWrite(LED_3, ((val & 0b0100) != 0));
  digitalWrite(LED_4, ((val & 0b1000) != 0));
}

void onReceive(int howMany) {
#ifdef DEBUG
  Serial.print("onReceive:");
#endif
  uint8_t receivedLen = 0;
  boolean changedLedsRegister = false;
  while (0 < Wire.available()) {
    uint8_t v = Wire.read();
#ifdef DEBUG
    Serial.print(" ");
    Serial.print(v, HEX);
#endif
    if (receivedLen == 0) {
      registerIndex = v;
    } else {
      if (registerIndex == WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS) {
        registers[registerIndex] = v;
        changedLedsRegister = true;
      }
      ++registerIndex;
    }
    ++receivedLen;
  }
  if (changedLedsRegister) {
    updateLeds();
  }
#ifdef DEBUG
  Serial.println("");
#endif
}

void onRequest() {
  if (registerIndex < REGISTER_LEN) {
    Wire.write(&registers[registerIndex], REGISTER_LEN - registerIndex);
  } else {
    Wire.write(0);
  }
}

void setup() {
  pinMode(BTN_TOP, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_BOTTOM, INPUT_PULLUP);
  pinMode(BTN_JOY, INPUT_PULLUP);
  pinMode(ADDR_JUMPER, INPUT_PULLUP);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);

#ifdef DEBUG
  Serial.begin(115200);
#endif

  registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS] = 1;
  updateLeds();

  Wire.begin(digitalRead(ADDR_JUMPER) == HIGH ? WIRED_CONTROLLER_ASUKIAAA_ADDRESS_DEFAULT : WIRED_CONTROLLER_ASUKIAAA_ADDRESS_JUMPER_CONNECTED);

  // Disable pull up for I2C pins
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);

  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
}

void updateJoystickRegisters() {
  int16_t joystickVertical = getJoystickVerticalValue();
  int16_t joystickHorizontal = getJoystickHorizontalValue();
  // Serial.println("vert: " + String(joystickVertical));
  // Serial.println("hori: " + String(joystickHorizontal));
  registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_UPPER] = joystickVertical >> 8;
  registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_LOWER] = joystickVertical & 0xff;
  registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_UPPER] = joystickHorizontal >> 8;
  registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_LOWER] = joystickHorizontal & 0xff;
}

void updateButtonRegister() {
  uint8_t val = 0x00;
  if (pushed(BTN_TOP))    val |= 0b0000001;
  if (pushed(BTN_LEFT))   val |= 0b0000010;
  if (pushed(BTN_RIGHT))  val |= 0b0000100;
  if (pushed(BTN_BOTTOM)) val |= 0b0001000;
  if (pushed(BTN_JOY))    val |= 0b0010000;
  registers[WIRED_CONTROLLER_ASUKIAAA_REGISTER_BUTTONS] = val;
}

void loop() {
  updateJoystickRegisters();
  updateButtonRegister();
  delay(20);
}
