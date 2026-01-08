#include <LedControl.h>

#define DIN 11
#define CLK 13
#define CS  10
#define NUM_MODULES 4   // change to 1 if single matrix

LedControl lc = LedControl(DIN, CLK, CS, NUM_MODULES);

// Diamond pattern
byte diamond[8] = {
  B00011000,
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

// Cross pattern
byte cross[8] = {
  B10000001,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B10000001
};

void setup() {
  for (int i = 0; i < NUM_MODULES; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 10);
    lc.clearDisplay(i);
  }
}

void showPattern(byte pattern[]) {
  for (int m = 0; m < NUM_MODULES; m++) {
    for (int row = 0; row < 8; row++) {
      lc.setRow(m, row, pattern[row]);
    }
  }
}

void loop() {
  showPattern(diamond);
  delay(1500);

  showPattern(cross);
  delay(1500);

  // Blink effect
  for (int i = 0; i < 3; i++) {
    for (int m = 0; m < NUM_MODULES; m++) lc.clearDisplay(m);
    delay(300);
    showPattern(diamond);
    delay(300);
  }
}
