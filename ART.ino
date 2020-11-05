/*
 * by Elijah Hewer
 * 
 * November 2020
 */

#include <LiquidCrystal.h>

// FSM states
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

// timing variables
int sec;
unsigned long msLast;

void loop() {
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
  int pot = readPot();
  
  // FSM
  switch (state) {
    case STATE_MENU:
      lcd.setCursor(0, 0);
      lcd.print("Round time:");
      
      switch (pot) {
        case 0:
          sec = 30;
          break;
        case 1:
          sec = 60;
          break;
        case 2:
          sec = 90;
          break;
        case 3:
          sec = 120;
          break;
        case 4:
          sec = 150;
          break;
        case 5:
          sec = 180;
          break;
        case 6:
          sec = 300;
          break;
        default:
          sec = 0;
          break;
      }

      displayTime(sec);

      // continue condition
      if (butG == HIGH) {
        clearLine(0);
        state = STATE_COUNT;
        msLast = millis();
      }
      
      break;
      
    case STATE_COUNT:
      lcd.setCursor(0, 0);
      lcd.print("GO!!");
      
      updateTime();
      displayTime(sec);

      // continue condition
      if (sec <= 0) {
        state = STATE_MENU;
      }
      
      break;
      
    case STATE_PAUSE:
      break;
      
    default:
      switch (pot) {
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;
        default:
          break;
      }
      break;
  }
}

// returns a value [0, 6] from the potentiometer
int readPot() {
  int val = analogRead(potPin);

  if (val >= 920) {
    return 6;
  } else if (val >= 800) {
    return 5;
  } else if (val >= 600) {
    return 4;
  } else if (val >= 400) {
    return 3;
  } else if (val >= 200){
    return 2;
  } else if (val >= 80){
    return 1;
  } else {
    return 0;
  }
}

// takes time in seconds and displays it in M:SS format
void displayTime(int sec) {
  char buff[4];
  sprintf(buff, "%i:%02i", sec / 60, sec % 60);
  
  lcd.setCursor(0,1);
  lcd.print(buff);
}

// updates global variables to represent changes in seconds
// unsigned long to handle millis() rollover
void updateTime() {
  unsigned long msCur = millis();
  
  if (msCur - msLast > 1000) {
    msLast = msCur;
    sec--;
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
