#include <TM1637NR.h>

// change according to wiring
const byte TM1637NR::CLOCK = 13;
const byte TM1637NR::DATA = 12;

static uint8_t number;

// letter h
const int SS_h = 116;

void setup() {
  number = 0;
  TM1637NR::reset();
  TM1637NR::displaySS(2, SS_h);
}

void loop() {
  TM1637NR::displayHex(0, number++);
  delay(200);
}
