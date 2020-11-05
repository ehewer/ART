#include <LiquidCrystal.h>

#define STATE_MENU 0
#define STATE_COUNT 1
#define STATE_PAUSE 2

int state = STATE_MENU;

// LCD pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// buzzer pin
const int buzPin = 9;
bool buz_state = false;

// pot pin
const int potPin = 0;

// button pins
const int butGPin = 7;
const int butRPin = 6;

void setup() {
  // LCD setup
  lcd.begin(16, 2);
  lcd.print("Setup...");

  // initialize pushbuttons
  pinMode(butGPin, INPUT_PULLUP);
  pinMode(butRPin, INPUT_PULLUP);
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
    tone(buzPin, 1000);
    buz_state = true;
  } else if (buz_state && curTime % 5 != 0) {
    noTone(buzPin);
    buz_state = false;
  }
  */

  // read from inputs
  int butG = digitalRead(butGPin);
  int butR = digitalRead(butRPin);
  int pot = readPot()
  
  // FSM
  switch (state) {
    case STATE_MENU:   
      switch (pot) {
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5
          break;
        default:
          break;
      }
      break;
    case STATE_COUNT:
      break;
    case STATE_PAUSE:
      break;
    default:
      break;
  }
}

// returns a value [1, 5] from the potentiometer
int readPot() {
  int val = analogRead(potPin);
  
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
