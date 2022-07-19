#include <WiredController_asukiaaa.h>
#include <M5Stack.h>

WiredController_asukiaaa controller(&Wire);
// WiredController_asukiaaa controller(&Wire, WIRED_CONTROLLER_ASUKIAAA_ADDRESS_JUMPER_CONNECTED); // when connected JP1
WiredController_asukiaaa_WriteInfo wInfo;
WiredController_asukiaaa_ReadInfo rInfo;

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2);
  // controller.useCRC(true);  // CRC is supported for
  // protocol version 1 or more
  Wire.begin();
  if (controller.write(wInfo) != 0) {
    Serial.println("Failed to initial writing");
  }
}

String padStart(String str, uint8_t len, char pad) {
  while (str.length() < len) {
    str = pad + str;
  }
  return str;
}

String getBooleanResultStr(bool target) {
  String str = target ? "true" : "false";
  return padStart(str, 5, ' ');
}

void loop() {
  int targetLedIndex = (millis() / 1000) % 4;
  wInfo.led1 = false;
  wInfo.led2 = false;
  wInfo.led3 = false;
  wInfo.led4 = false;
  switch(targetLedIndex) {
  case 0:
    wInfo.led1 = true;
    break;
  case 1:
    wInfo.led2 = true;
    break;
  case 2:
    wInfo.led3 = true;
    break;
  case 3:
    wInfo.led4 = true;
    break;
  }

  M5.Lcd.setCursor(0,0);
  if (controller.write(wInfo) == 0) {
    M5.Lcd.println("Turn on led " + String(targetLedIndex + 1));
  } else {
    M5.Lcd.println("Cannot write info");
  }

  if (controller.read(&rInfo) == 0) {
    M5.Lcd.println("JoystickHorizontal: " + padStart(String(rInfo.joystickHorizontal), 4, ' '));
    M5.Lcd.println("JoystickVertical:   " + padStart(String(rInfo.joystickVertical), 4, ' '));
    M5.Lcd.println("btnTop:    " + getBooleanResultStr(rInfo.btnTop));
    M5.Lcd.println("btnLeft:   " + getBooleanResultStr(rInfo.btnLeft));
    M5.Lcd.println("btnRight:  " + getBooleanResultStr(rInfo.btnRight));
    M5.Lcd.println("btnBottom: " + getBooleanResultStr(rInfo.btnBottom));
    M5.Lcd.println("btnJoy:    " + getBooleanResultStr(rInfo.btnJoy));
  } else {
    M5.Lcd.println("Cannot read info");
  }

  M5.Lcd.println("At " + String(millis()));
  delay(100);
}
