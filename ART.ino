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
  /*
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  int curTime = millis() / 1000;
  lcd.print(curTime);
  */
  /*
  if (!buz_state && curTime % 5 == 0) {
    tone(buz, 1000);
    buz_state = true;
  } else if (buz_state && curTime % 5 != 0) {
    noTone(buz);
    buz_state = false;
  }
  */
  lcd.setCursor(0, 1);
  lcd.print(readPot());
}

int readPot() {
  int val = analogRead(pot);
  
  if (val >= 800) {
    return 5;
  } else if (val >= 600) {
    return 4;
  } else if (val >= 400) {
    return 3;
  } else if (val >= 200){
    return 2;
  } else {
    return 1;
  }
}

// clears a single line (row) on the LCD
void clearLine(int line) {
  int columns = 16;
  lcd.setCursor(0, line);
  for(int i=0; i < columns; i++) {
    lcd.print(" ");
  }
}
