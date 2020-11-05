#include <LiquidCrystal.h>

// LCD pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// buzzer pin
const int buz = 9;
bool buz_state = false;

// pot pin
const int pot = 0;

void setup() {
  // LCD setup
  lcd.begin(16, 2);
  lcd.print("Hello World!");
}

void loop() {
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  int curTime = millis() / 1000;
  lcd.print(curTime);
  
  /*
  if (!buz_state && curTime % 5 == 0) {
    tone(buz, 1000);
    buz_state = true;
  } else if (buz_state && curTime % 5 != 0) {
    noTone(buz);
    buz_state = false;
  }
  */

  lcd.setCursor(0, 0);
  lcd.print(readPot());
}

int readPot() {
  return analogRead(pot);
}
