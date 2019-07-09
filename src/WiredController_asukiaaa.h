#ifndef WIRED_CONTROLLER_ASUKIAAA_H
#define WIRED_CONTROLLER_ASUKIAAA_H
#include <Wire.h>

// #define DEBUG

#ifdef DEBUG
#include <Arduino.h>
#endif

#define WIRED_CONTROLLER_ASUKIAAA_ADDRESS 0x08
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_LEDS           0x00
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_BUTTONS        0x01
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_UPPER 0x02
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_VERT_LOWER 0x03
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_UPPER 0x04
#define WIRED_CONTROLLER_ASUKIAAA_REGISTER_JOY_HORI_LOWER 0x05
#define JOYSTICK_MAX_VALUE 1000

#define WIRED_CONTROLLER_ASUKIAAA_CANNOT_READ 5

typedef struct {
  uint16_t joystickHorizontal;
  uint16_t joystickVertical;
  bool btn1;
  bool btn2;
} WiredController_asukiaaa_ReadInfo;

typedef struct {
  bool led1;
} WiredController_asukiaaa_WriteInfo;

class WiredController_asukiaaa {
 public:
  WiredController_asukiaaa(TwoWire* wire);
  uint8_t writeInfo(WiredController_asukiaaa_WriteInfo wInfo);
  uint8_t readInfo(WiredController_asukiaaa_ReadInfo *rInfo);

 private:
  TwoWire* wire;
};

#endif
