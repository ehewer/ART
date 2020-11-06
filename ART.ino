/*
 * Arduino Round Timer (ART)
 * Created by Elijah Hewer
 * 
 * November 2020
 */

#include <LiquidCrystal.h>

// FSM states
#define STATE_SETUP0 0
#define STATE_SETUP1 1
#define STATE_SETUP2 2
#define STATE_ROUND 3
#define STATE_REST 4
#define STATE_PAUSE 5

int state = STATE_SETUP0;

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
// posedge detection
bool butGPosedge = false;
bool butRPosedge = false;
// debounce variables
const unsigned long msBounce = 30; // duration of debounce in ms

// timing variables
int secRemain;
int secRound;
int secRest;
int roundCur;
int roundMax;
unsigned long msStart;

void setup() {
  // LCD setup
  lcd.begin(16, 2);
  displayState();

  // initialize pushbuttons
  pinMode(butGPin, INPUT_PULLUP);
  pinMode(butRPin, INPUT_PULLUP);
}

void loop() {
  // DON'T call displayState() from here!
  // While it would work and result in less code,
  // we only want to print on transition - uses less resources
  
  // read from inputs
  updatePosedge();
  int pot = readPot();
  
  // FSM
  switch (state) {
    // Round time
    case STATE_SETUP0:
      switch (pot) {
        case 0:
          secRound = 30;
          break;
        case 1:
          secRound = 60;
          break;
        case 2:
          secRound = 90;
          break;
        case 3:
          secRound = 120;
          break;
        case 4:
          secRound = 150;
          break;
        case 5:
          secRound = 180;
          break;
        case 6:
          secRound = 300;
          break;
        default:
          secRound = 0;
          break;
      }

      displayTime(secRound);

      // continue condition
      if (butGPosedge) {
        state = STATE_SETUP1;
        displayState();
      }
      
      break;

    // Rest time
    case STATE_SETUP1:
      switch (pot) {
        case 0:
          secRest = 15;
          break;
        case 1:
          secRest = 30;
          break;
        case 2:
          secRest = 45;
          break;
        case 3:
          secRest = 60;
          break;
        case 4:
          secRest = 75;
          break;
        case 5:
          secRest = 90;
          break;
        case 6:
          secRest = 120;
          break;
        default:
          secRest = 0;
          break;
      }
      
      displayTime(secRest);

      // continue condition
      if (butGPosedge) {
        state = STATE_SETUP2;
        displayState();
      }
      // return condition
      else if (butRPosedge) {
        state = STATE_SETUP0;
        displayState();
      }
      
      break;

    // # of Rounds
    case STATE_SETUP2:
      switch (pot) {
        case 0:
          roundMax = 1;
          break;
        case 1:
          roundMax = 3;
          break;
        case 2:
          roundMax = 4;
          break;
        case 3:
          roundMax = 6;
          break;
        case 4:
          roundMax = 9;
          break;
        case 5:
          roundMax = 12;
          break;
        case 6:
          roundMax = 99;
          break;
        default:
          roundMax = 0;
          break;
      }

      // displayRoundMax();
      lcd.setCursor(0,1);
      lcd.print("ROUND 1");
      
      // continue condition
      if (butGPosedge) {
        state = STATE_ROUND;
        roundCur = 1;
        displayState();
        msStart = millis();
      }
      // return condition
      else if (butRPosedge) {
        state = STATE_SETUP1;
        displayState();
      }
      
      break;
      
    case STATE_ROUND:
      updateTime(secRound);
      displayTime(secRemain);

      /*
      // rest condition
      if (secRemain <= 0) {
        state = STATE_SETUP0;
        displayState();
      }
      
      // pause condition
      else */
      if (butRPosedge) {
        state = STATE_PAUSE;
        displayState();
      }
      
      break;
      
    case STATE_PAUSE:
      lcd.setCursor(11, 1);
      lcd.print("CONF");
      /*
      // resume condition
      if (butGPosedge) {
        state = STATE_ROUND;
        displayState();
        secRound = secRemain;
        msStart = millis();
      }
      // finish condition
      else if (butRPosedge) {
        state = STATE_SETUP0;
        displayState();
      }
      */
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

// Returns a value [0, 6] based off of the analogue
// value read from the 10k potentiometer
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

// Detects posedges on pushbuttons with debounce
void updatePosedge() {
  int butG = digitalRead(butGPin);
  int butR = digitalRead(butRPin);
  static int butGLast = LOW;
  static int butRLast = LOW;
  static unsigned long msGLast = 0;
  static unsigned long msRLast = 0;

  // if falling edge, update time of last fall
  if (butG == LOW && butGLast == HIGH) {
    msGLast = millis();
    butGPosedge = false;
  }
  // if rising edge, check time since last fall
  else if (butG == HIGH && butGLast == LOW && (millis() - msGLast > msBounce)) {
    butGPosedge = true;
  } else {
    butGPosedge = false;
  }

  // if falling edge, update time of last fall
  if (butR == LOW && butRLast == HIGH) {
    msRLast = millis();
    butRPosedge = false;
  }
  // if rising edge, check time since last fall
  else if (butR == HIGH && butRLast == LOW && (millis() - msRLast > msBounce)) {
    butRPosedge = true;
  } else {
    butRPosedge = false;
  }
  
  butGLast = butG;
  butRLast = butR;
}

// Decrements global variable "secRemain" approximately every second
// Uses unsigned long to handle millis() rollover
// Input: duration of round or rest in seconds
void updateTime(int duration) { 
  unsigned long msCur = millis();
  secRemain = duration - ((msCur - msStart) / 1000);
}

// Displays input on second line of LCD in M:SS format
// Input: seconds to display
void displayTime(int sec) {
  char buff[4];
  sprintf(buff, "%i:%02i", sec / 60, sec % 60);

  lcd.setCursor(0,1);
  lcd.print(buff);
}

// Displays max round on second line of LCD
void displayRoundMax() {
  char bu[5];
  sprintf(bu, "%-2i", roundMax);

  lcd.setCursor(0, 1);
  lcd.print(bu);
}

// Displays the appropriate message on the first line of LCD
// based on the current state (if appropriate);
void displayState() {
  lcd.clear();

  char buff[13];
  
  switch (state){
    case STATE_SETUP0:
      lcd.print("Round time:");
      break;
    case STATE_SETUP1:
      lcd.print("Rest time:");
      break;
    case STATE_SETUP2:
      lcd.print("# of Rounds:");
      break;
    case STATE_ROUND:
      sprintf(buff, "Round %2i / %-2i", roundCur, roundMax);
      lcd.print(buff);
      break;
    case STATE_PAUSE:
      sprintf(buff, "Round %2i / %-2i", roundCur, roundMax);
      lcd.print(buff);
      displayTime(secRemain);
      lcd.setCursor(5, 1);
      lcd.print("PAUSED");
      break;
    default:
      break;
  }
}

// CURRENTLY UNUSED
// Clears a single line (row) on the LCD
// Input: index of row to clear
void clearLine(int line) {
  int columns = 16;
  lcd.setCursor(0, line);
  for(int i=0; i < columns; i++) {
    lcd.print(" ");
  }
}
