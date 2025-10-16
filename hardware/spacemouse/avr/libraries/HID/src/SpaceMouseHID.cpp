#include "SpaceMouseHID.h"

const uint8_t SpaceMouseHID_::_hidReportDescriptor[] PROGMEM = {
  0x05, 0x01, 0x09, 0x08, 0xA1, 0x01,
  0xA1, 0x00, 0x85, 0x01, 0x16, 0x00, 0x80, 0x26, 0xFF, 0x7F,
  0x09, 0x30, 0x09, 0x31, 0x09, 0x32, 0x75, 0x10, 0x95, 0x03, 0x81, 0x02,
  0xC0,
  0xA1, 0x00, 0x85, 0x02, 0x16, 0x00, 0x80, 0x26, 0xFF, 0x7F,
  0x09, 0x33, 0x09, 0x34, 0x09, 0x35, 0x75, 0x10, 0x95, 0x03, 0x81, 0x02,
  0xC0, 0xC0
};

HIDSubDescriptor SpaceMouseHID_::node(_hidReportDescriptor, sizeof(_hidReportDescriptor));

void SpaceMouseHID_::begin() {
  HID().AppendDescriptor(&node);
  readAll(centerPoints);
  delay(50);
  readAll(centerPoints);
}

void SpaceMouseHID_::readAll(int *rawReads) {
  for (int i = 0; i < 8; i++) rawReads[i] = analogRead(PINLIST[i]);
}

void SpaceMouseHID_::sendCommand(int16_t rx, int16_t ry, int16_t rz,
                                 int16_t x, int16_t y, int16_t z) {
  uint8_t trans[6] = {x & 0xFF, x >> 8, y & 0xFF, y >> 8, z & 0xFF, z >> 8};
  HID().SendReport(1, trans, 6);
  uint8_t rot[6] = {rx & 0xFF, rx >> 8, ry & 0xFF, ry >> 8, rz & 0xFF, rz >> 8};
  HID().SendReport(2, rot, 6);
}

void SpaceMouseHID_::update() {
  int raw[8], centered[8];
  readAll(raw);

  for (int i = 0; i < 8; i++) {
    centered[i] = raw[i] - centerPoints[i];
    if (abs(centered[i]) < DEADZONE) centered[i] = 0;
  }

  int16_t transX, transY, transZ, rotX, rotY, rotZ;
  transX = -(-centered[5] + centered[1]);
  transY = (-centered[3] + centered[7]);
  if ((abs(centered[0]) > DEADZONE) &&
      (abs(centered[2]) > DEADZONE) &&
      (abs(centered[4]) > DEADZONE) &&
      (abs(centered[6]) > DEADZONE)) {
    transZ = (-centered[0] - centered[2] - centered[4] - centered[6]);
    transX = 0; transY = 0;
  } else transZ = 0;

  rotX = (-centered[0] + centered[4]);
  rotY = (+centered[2] - centered[6]);
  if ((abs(centered[1]) > DEADZONE) &&
      (abs(centered[3]) > DEADZONE) &&
      (abs(centered[5]) > DEADZONE) &&
      (abs(centered[7]) > DEADZONE)) {
    rotZ = (+centered[1] + centered[3] + centered[5] + centered[7]) / 2;
    rotX = 0; rotY = 0;
  } else rotZ = 0;

  if (invX) transX *= -1;
  if (invY) transY *= -1;
  if (invZ) transZ *= -1;
  if (invRX) rotX *= -1;
  if (invRY) rotY *= -1;
  if (invRZ) rotZ *= -1;

  sendCommand(rotX, rotZ, rotY, transX, transZ, transY);
}

// Create global instance
SpaceMouseHID_ SpaceMouse;
