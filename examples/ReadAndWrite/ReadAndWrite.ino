#include <WiredController_asukiaaa.h>

#define WRITE_INTERVAL_MS 1000

unsigned long wroteAt = 0;

WiredController_asukiaaa controller(&Wire);
// WiredController_asukiaaa controller(&Wire, WIRED_CONTROLLER_ASUKIAAA_ADDRESS_JUMPER_CONNECTED); // when connected JP1
WiredController_asukiaaa_WriteInfo wInfo;
WiredController_asukiaaa_ReadInfo rInfo;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  wroteAt = millis();
  wInfo.led1 = false;
  wroteAt = millis();
  if (controller.write(wInfo) != 0) {
    Serial.println("Failed to initial writing");
  }
}

void printlnBooleanResult(bool target) {
  if (target) {
    Serial.println("true");
  } else {
    Serial.println("false");
  }
}

uint8_t ledCount = 0;

void loop() {
  if (millis() - wroteAt > WRITE_INTERVAL_MS) {
    wroteAt = millis();
    ++ledCount;
    if (ledCount > 4) ledCount = 0;
    wInfo.led1 = false;
    wInfo.led2 = false;
    wInfo.led3 = false;
    wInfo.led4 = false;
    switch(ledCount) {
    case 1:
      wInfo.led1 = true;
      break;
    case 2:
      wInfo.led2 = true;
      break;
    case 3:
      wInfo.led3 = true;
      break;
    case 4:
      wInfo.led4 = true;
      break;
    }
    if (controller.write(wInfo) == 0) {
      Serial.println("Wrote info to controller.");
    } else {
      Serial.println("Cannot write info to controller.");
    }
  }
  if (controller.read(&rInfo) == 0) {
    Serial.println("JoystickHorizontal: " + String(rInfo.joystickHorizontal));
    Serial.println("JoystickVertical: " + String(rInfo.joystickVertical));
    Serial.print("btnTop: ");
    printlnBooleanResult(rInfo.btnTop);
    Serial.print("btnLeft: ");
    printlnBooleanResult(rInfo.btnLeft);
    Serial.print("btnRight: ");
    printlnBooleanResult(rInfo.btnRight);
    Serial.print("btnBottom: ");
    printlnBooleanResult(rInfo.btnBottom);
    Serial.print("btnJoy: ");
    printlnBooleanResult(rInfo.btnJoy);
  } else {
    Serial.println("Cannot read info from controller.");
  }
  delay(100);
}
