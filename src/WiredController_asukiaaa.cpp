#include "WiredController_asukiaaa.h"

WiredController_asukiaaa::WiredController_asukiaaa(TwoWire* wire, int address) {
  this->wire = wire;
  this->address = address;
}

uint8_t WiredController_asukiaaa::write(WiredController_asukiaaa_WriteInfo wInfo) {
  uint8_t ledState = 0;
  if (wInfo.led1) ledState |= 0b0001;
  if (wInfo.led2) ledState |= 0b0010;
  if (wInfo.led3) ledState |= 0b0100;
  if (wInfo.led4) ledState |= 0b1000;

  wire->beginTransmission(address);
  wire->write(WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS);
  wire->write(ledState);
  return wire->endTransmission();
}

int WiredController_asukiaaa::read(WiredController_asukiaaa_ReadInfo *rInfo) {
  wire->beginTransmission(address);
  wire->write(WIRED_CONTROLLER_ASUKIAAA_REGISTER_BUTTONS);
  rInfo->stateRead = wire->endTransmission();
  if (rInfo->stateRead != 0) {
    return rInfo->stateRead;
  }
  static const uint8_t buffLen = 5;
  uint8_t buff[buffLen];
  uint8_t buffIndex = 0;
  uint8_t receiveLen = wire->requestFrom(address, (int) buffLen);

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
    rInfo->stateRead = WIRED_CONTROLLER_ASUKIAAA_CANNOT_READ;
    return rInfo->stateRead;
  }

  uint8_t buttons = buff[0];
  uint16_t vertValue = ((uint16_t) buff[1]) << 8 | (uint16_t) buff[2];
  uint16_t horiValue = ((uint16_t) buff[3]) << 8 | (uint16_t) buff[4];
  if (vertValue > WIRED_CONTROLLER_ASUKIAAA_JOYSTICK_MAX_VALUE || horiValue > WIRED_CONTROLLER_ASUKIAAA_JOYSTICK_MAX_VALUE) {
    rInfo->stateRead = WIRED_CONTROLLER_ASUKIAAA_BROKEN_DATA;
    return rInfo->stateRead;
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

  return rInfo->stateRead;
}
