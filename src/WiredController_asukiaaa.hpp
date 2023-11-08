#ifndef WIRED_CONTROLLER_ASUKIAAA_H
#define WIRED_CONTROLLER_ASUKIAAA_H

#include <CRCx.h>
#include <Wire.h>
#include <wire_asukiaaa.h>

#define WIRED_CONTROLLER_ASUKIAAA_ADDRESS_DEFAULT 0x20
#define WIRED_CONTROLLER_ASUKIAAA_ADDRESS_JUMPER_CONNECTED 0x21
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS 0x00
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_BUTTONS 0x01
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_UPPER 0x02
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_LOWER 0x03
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_UPPER 0x04
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_LOWER 0x05
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_PROTOCOL_VERSION 0x06
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_CRC 0x07

#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_LENGTH 0x09
#define WIRED_CONTROLLER_ASUKIAAA_JOYSTICK_MAX_VALUE 1000

#define WIRED_CONTROLLER_ASUKIAAA_CANNOT_READ 5
#define WIRED_CONTROLLER_ASUKIAAA_BROKEN_DATA 6
#define WIRED_CONTROLLER_ASUKIAAA_UNMATCH_CRC 10

typedef struct {
  uint16_t joystickHorizontal;
  uint16_t joystickVertical;
  bool btnTop;
  bool btnLeft;
  bool btnRight;
  bool btnBottom;
  bool btnJoy;
  int stateRead = -1;
  uint8_t protocolVersion = 0;
} WiredController_asukiaaa_ReadInfo;

typedef struct {
  bool led1;
  bool led2;
  bool led3;
  bool led4;
} WiredController_asukiaaa_WriteInfo;

class WiredController_asukiaaa {
 public:
  TwoWire* wire;
  const int address;

  WiredController_asukiaaa(
      TwoWire* wire, int address = WIRED_CONTROLLER_ASUKIAAA_ADDRESS_DEFAULT)
      : address(address) {
    this->wire = wire;
  }

  void useCRC(bool useCRC) { this->usingCRC = useCRC; }

  uint8_t write(WiredController_asukiaaa_WriteInfo wInfo) {
    uint8_t ledState = 0;
    if (wInfo.led1) ledState |= 0b0001;
    if (wInfo.led2) ledState |= 0b0010;
    if (wInfo.led3) ledState |= 0b0100;
    if (wInfo.led4) ledState |= 0b1000;

    return wire_asukiaaa::writeBytes(
        wire, address, WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS, &ledState, 1);
  }

  int read(WiredController_asukiaaa_ReadInfo* rInfo) {
    rInfo->stateRead =
        wire_asukiaaa::readBytes(wire, address, 0, buff, buffLen);
    if (rInfo->stateRead != 0) {
      return rInfo->stateRead;
    }

    if (usingCRC &&
        ((uint16_t)buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_CRC] << 8 |
         buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_CRC + 1]) !=
            crcx::crc16(buff, WIRED_CONTROLLER_ASUKIAAA_REGISTER_CRC)) {
      return rInfo->stateRead = WIRED_CONTROLLER_ASUKIAAA_UNMATCH_CRC;
    }

    uint8_t buttons = buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_BUTTONS];
    uint16_t vertValue =
        ((uint16_t)buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_UPPER])
            << 8 |
        (uint16_t)buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_LOWER];
    uint16_t horiValue =
        ((uint16_t)buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_UPPER])
            << 8 |
        (uint16_t)buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_LOWER];
    if (vertValue > WIRED_CONTROLLER_ASUKIAAA_JOYSTICK_MAX_VALUE ||
        horiValue > WIRED_CONTROLLER_ASUKIAAA_JOYSTICK_MAX_VALUE) {
      rInfo->stateRead = WIRED_CONTROLLER_ASUKIAAA_BROKEN_DATA;
      return rInfo->stateRead;
    }

    rInfo->btnTop = ((buttons & 0b00000001) != 0);
    rInfo->btnLeft = ((buttons & 0b00000010) != 0);
    rInfo->btnRight = ((buttons & 0b00000100) != 0);
    rInfo->btnBottom = ((buttons & 0b00001000) != 0);
    rInfo->btnJoy = ((buttons & 0b00010000) != 0);
    rInfo->joystickVertical = vertValue;
    rInfo->joystickHorizontal = horiValue;
    rInfo->protocolVersion =
        buff[WIRED_CONTROLLER_ASUKIAAA_REGISTER_PROTOCOL_VERSION];

    return rInfo->stateRead;
  }

 private:
  bool usingCRC = false;
  static const uint8_t buffLen = WIRED_CONTROLLER_ASUKIAAA_REGISTER_LENGTH;
  uint8_t buff[buffLen];
};

#endif
