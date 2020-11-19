/* Ralph Doncaster 2020
 * TM1637NR.h - control TM1637-based LED display modules
 * based on TM1637NR github.com/nerdralph/TM1638NR
 * MIT license.
 * 20201115 v0.5
 */

#pragma once

/*
LED & Key module segments:
  -0-
 5   1
  -6-
 4   2
  -3- .7
*/

static const int8_t HEXSS[] PROGMEM = {
  0x3F, /* 0 */
  0x06, /* 1 */
  0x5B, /* 2 */
  0x4F, /* 3 */
  0x66, /* 4 */
  0x6D, /* 5 */
  0x7D, /* 6 */
  0x07, /* 7 */
  0x7F, /* 8 */
  0x6F, /* 9 */
  0x77, /* A */
  0x7C, /* B */
  0x39, /* C */
  0x5E, /* D */
  0x79, /* E */
  0x71, /* F */
};

class TM1637NR {
 public:
  // brightness levels 0-7
  static void reset(uint8_t brightness = 2){
    setbright(brightness);
    // initialize display data (address 0x00 to 0x06) to 0
    uint8_t i = 6;
    do {
      writeLoc(i, 0);
    } while (i--);
  }

  static void setbright(uint8_t brightness){
    start();
    send(DISPLAY_ON | (brightness & 0x07));
    stop();
  }

  // write to seven segments
  static void displaySS(uint8_t position, uint8_t value) {
    writeLoc(position, value);
  }

  // display lower nibble as hex (0-F)
  static void displayNibble(uint8_t position, uint8_t nibble) {
    displaySS(position, pgm_read_byte(HEXSS + (nibble & 0x0F)));
  }

  // display binary u8 as 2-digit hex number
  static void displayHex(uint8_t position, uint8_t value) {
    displayNibble(position, value / 16);
    displayNibble(position + 1, value);
  }

  static void start() {
    // make sure DATA is high first
    while (digitalRead(DATA) == 0);
    delayMicroseconds(5);
    pinMode(DATA, OUTPUT);
    delayMicroseconds(5);
  }

  static void stop() {
    //pinMode(CLOCK, OUTPUT);
    clkLow();
    pinMode(DATA, OUTPUT);
    pinMode(CLOCK, INPUT);
    // wait for rise time
    while (digitalRead(CLOCK) == 0);
    delayMicroseconds(5);
    pinMode(DATA, INPUT);             // release data line
  }

  // open-drain output, data clocked on rising edge, LSB first 1Mhz max
  // receive/transmit
  static void send(uint8_t data) {
    uint8_t bits = 8;
    do {
      //pinMode(CLOCK, OUTPUT);
      clkLow();
      pinMode(DATA, INPUT);             // release data line
      if (!(data & 0x01)) pinMode(DATA, OUTPUT);
      data /= 2;
      pinMode(CLOCK, INPUT);
      // wait for rise time
      while (digitalRead(CLOCK) == 0);
      delayMicroseconds(5);
    }
    while (--bits);

    //pinMode(CLOCK, OUTPUT);
    clkLow();
    pinMode(DATA, INPUT);             // release data line
    pinMode(CLOCK, INPUT);
    // wait for ack
    while (digitalRead(DATA) == 1);
    delayMicroseconds(5);
  }

  // set address (0-6), then send data to write
  static void writeLoc(uint8_t position, uint8_t value) {
    start();
    send(SET_ADDRESS | position);
    send(value);
    stop();
  }

  static const byte CLOCK;
  static const byte DATA;

  private:

  static void clkLow() { 
      pinMode(CLOCK, OUTPUT);
      delayMicroseconds(2.5);
  }

  enum COMMAND {
    READ_KEYS = 0x42,                   // datasheet pg 4 table 1
    DISPLAY_ON = 0x88,                  // datasheet pg 5 table 3
    SET_ADDRESS = 0xC0                  // datasheet pg 5 table 2
  };

};

