#include <LedControl.h>

LedControl lc = LedControl(11, 13, 10, 4);

byte two[8] = {
  B00111100,
  B01000010,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01111110,
  B00000000
};

byte zero[8] = {
  B00111100,
  B01000010,
  B01000110,
  B01001010,
  B01010010,
  B01100010,
  B00111100,
  B00000000
};

byte six[8] = {
  B00111100,
  B01000000,
  B01000000,
  B01111100,
  B01000010,
  B01000010,
  B00111100,
  B00000000
};

void setup() {
  for (int i = 0; i < 4; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 10);
    lc.clearDisplay(i);
  }

  for (int r = 0; r < 8; r++) {
    lc.setRow(3, r, two[r]);
    lc.setRow(2, r, zero[r]);
    lc.setRow(1, r, two[r]);
    lc.setRow(0, r, six[r]);
  }
}

void loop() {}
