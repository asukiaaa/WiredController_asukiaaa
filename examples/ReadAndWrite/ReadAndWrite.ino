#include <WiredController_asukiaaa.h>

#define WIRE_TO_USE Wire
WiredController_asukiaaa controller(&WIRE_TO_USE);
// WiredController_asukiaaa controller(&Wire,
// WIRED_CONTROLLER_ASUKIAAA_ADDRESS_JUMPER_CONNECTED); // when connected JP1
WiredController_asukiaaa_WriteInfo wInfo;
WiredController_asukiaaa_ReadInfo rInfo;

void setup() {
  Serial.begin(115200);
  WIRE_TO_USE.begin();
  // controller.useCRC(true);  // CRC is supported for
  // protocol version 1 or more
}

String getBooleanResultStr(bool target) { return target ? "true" : "false"; }

void loop() {
  int targetLedIndex = (millis() / 1000) % 4;
  wInfo.led1 = false;
  wInfo.led2 = false;
  wInfo.led3 = false;
  wInfo.led4 = false;
  switch (targetLedIndex) {
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

  if (controller.write(wInfo) == 0) {
    Serial.println("Wrote info to turn on led " + String(targetLedIndex + 1));
  } else {
    Serial.println("Cannot write info to controller.");
  }

  if (controller.read(&rInfo) == 0) {
    Serial.println("JoystickHorizontal: " + String(rInfo.joystickHorizontal));
    Serial.println("JoystickVertical: " + String(rInfo.joystickVertical));
    Serial.println("btnTop: " + getBooleanResultStr(rInfo.btnTop));
    Serial.println("btnLeft: " + getBooleanResultStr(rInfo.btnLeft));
    Serial.println("btnRight: " + getBooleanResultStr(rInfo.btnRight));
    Serial.println("btnBottom: " + getBooleanResultStr(rInfo.btnBottom));
    Serial.println("btnJoy: " + getBooleanResultStr(rInfo.btnJoy));
    Serial.println("protocolVersion:" + String(rInfo.protocolVersion));
  } else {
    Serial.println("Cannot read info from controller. Error: " +
                   String(rInfo.stateRead));
  }

  Serial.println("At " + String(millis()));
  delay(100);
}
