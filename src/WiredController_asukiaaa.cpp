#include "WiredController_asukiaaa.h"

WiredController_asukiaaa::WiredController_asukiaaa(TwoWire* wire) {
  this->wire = wire;
}

uint8_t WiredController_asukiaaa::write(WiredController_asukiaaa_WriteInfo wInfo) {
  wire->beginTransmission(WIRED_CONTROLLER_ASUKIAAA_ADDRESS);
  wire->write(WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS);
  wire->write(wInfo.led1 ? 1 : 0);
  return wire->endTransmission();
}

uint8_t WiredController_asukiaaa::read(WiredController_asukiaaa_ReadInfo *rInfo) {
  wire->beginTransmission(WIRED_CONTROLLER_ASUKIAAA_ADDRESS);
  wire->write(WIRED_CONTROLLER_ASUKIAAA_REGISTER_BUTTONS);
  uint8_t result = wire->endTransmission();
  if (result != 0) {
    return result;
  }
  static const uint8_t buffLen = 5;
  uint8_t buff[buffLen];
  uint8_t buffIndex = 0;
  uint8_t receiveLen = wire->requestFrom(WIRED_CONTROLLER_ASUKIAAA_ADDRESS, (int) buffLen);

#ifdef DEBUG
  Serial.print("received:");
#endif
  while (wire->available() > 0) {
    uint8_t d = wire->read();
    if (buffIndex < buffLen) {
      buff[buffIndex] = d;
      ++buffIndex;
    }
#ifdef DEBUG
    Serial.print(" ");
    Serial.print(d, HEX);
#endif
  }
#ifdef DEBUG
  Serial.println("");
#endif

  if (receiveLen < buffLen) {
    return WIRED_CONTROLLER_ASUKIAAA_CANNOT_READ;
  }

  uint8_t buttons = buff[0];
  uint16_t vertValue = ((uint16_t) buff[1]) << 8 | (uint16_t) buff[2];
  uint16_t horiValue = ((uint16_t) buff[3]) << 8 | (uint16_t) buff[4];
  if (vertValue > JOYSTICK_MAX_VALUE || horiValue > JOYSTICK_MAX_VALUE) {
    return WIRED_CONTROLLER_ASUKIAAA_BROKEN_DATA;
  }

  rInfo->btnTop     = ((buttons & 0b00000001) != 0);
  rInfo->btnLeft    = ((buttons & 0b00000010) != 0);
  rInfo->btnRight   = ((buttons & 0b00000100) != 0);
  rInfo->btnBottom  = ((buttons & 0b00001000) != 0);
  rInfo->btnJoy     = ((buttons & 0b00010000) != 0);
  rInfo->joystickVertical = vertValue;
  rInfo->joystickHorizontal = horiValue;

#ifdef DEBUG
  Serial.println("joystick");
  Serial.println(rInfo->joystickVertical);
  Serial.println(rInfo->joystickHorizontal);
#endif

  return 0;
}
