#include <WiredController_asukiaaa.h>

#define WRITE_INTERVAL_MS 1000

unsigned long wroteAt = 0;

WiredController_asukiaaa controller(&Wire);
WiredController_asukiaaa_WriteInfo wInfo;
WiredController_asukiaaa_ReadInfo rInfo;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  wroteAt = millis();
  wInfo.led1 = false;
  wroteAt = millis();
  if (controller.writeInfo(wInfo) != 0) {
    Serial.println("Failed to initial writing");
  }
}

void loop() {
  if (millis() - wroteAt > WRITE_INTERVAL_MS) {
    wroteAt = millis();
    wInfo.led1 = !wInfo.led1;
    if (controller.writeInfo(wInfo) == 0) {
      Serial.println("Wrote info to controller.");
    } else {
      Serial.println("Cannot write info to controller.");
    }
  }
  if (controller.readInfo(&rInfo) == 0) {
    Serial.println("JoystickHorizontal: " + String(rInfo.joystickHorizontal));
    Serial.println("JoystickVertical: " + String(rInfo.joystickVertical));
    Serial.print("btn1: ");
    if (rInfo.btn1) {
      Serial.println("true");
    } else {
      Serial.println("false");
    }
    Serial.print("btn2: ");
    if (rInfo.btn1) {
      Serial.println("true");
    } else {
      Serial.println("false");
    }
  } else {
    Serial.println("Cannot read info from controller.");
  }
  delay(50);
}
