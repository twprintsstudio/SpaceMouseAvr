#pragma once
#include "HID.h"
#include <Arduino.h>

class SpaceMouseHID_ {
public:
  void begin();
  void update();

  // Optional tuning parameters
  bool invX  = false;
  bool invY  = false;
  bool invZ  = true;
  bool invRX = true;
  bool invRY = false;
  bool invRZ = true;
  int DEADZONE = 1;

private:
  static const uint8_t _hidReportDescriptor[];
  static HIDSubDescriptor node;

  void readAll(int *rawReads);
  void sendCommand(int16_t rx, int16_t ry, int16_t rz,
                   int16_t x, int16_t y, int16_t z);

  int centerPoints[8];
  int PINLIST[8] = { A1, A0, A3, A2, A7, A6, A9, A8 };
};

// Declare a global instance
extern SpaceMouseHID_ SpaceMouse;
