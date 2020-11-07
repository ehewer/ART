/*
 * Arduino Round Timer (ART) v1.0.1
 * Created by Elijah Hewer
 * https://www.linkedin.com/in/elijahhewer/
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
#define STATE_ROUND_PAUSE 5
#define STATE_REST_PAUSE 6

int state = STATE_SETUP0;

// LCD pins
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// buzzer pin
const int buzPin = 9;

// pot pin (analogue)
const int potPin = 0;

// button pins
const int butGPin = 7;
const int butRPin = 6;
// debounce variables
const unsigned long msBounce = 30; // duration of debounce in ms

// timing variables
int secRemain;
int secRound;
int secRest;
int roundCur;
int roundMax;
unsigned long msStart;

// track last displayed value for changes to trigger LCD updates
// set lastVal outside of appropriate range to guarantee LCD update
int lastVal = -1;

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
  bool posedge[] = {false, false}; // posedge[0] = green pushbutton, posedge[1] = red pushbutton
  updatePosedge(posedge);
  int pot = readPot();
  
  // FSM
  switch (state) {
    // set round time
    case STATE_SETUP0:
      if (pot != lastVal) {
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
      }
      
      // continue condition
      if (posedge[0]) {
        state = STATE_SETUP1;
        displayState();
        pot = -1;
      }

      // update last pot value
      lastVal = pot;
      
      break;

    // set rest time
    case STATE_SETUP1:
      if (pot != lastVal) {
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
      }

      // continue condition
      if (posedge[0]) {
        state = STATE_SETUP2;
        displayState();
        pot = -1;
      }
      // return condition
      else if (posedge[1]) {
        state = STATE_SETUP0;
        displayState();
        pot = -1;
      }

      // update last pot value
      lastVal = pot;
      
      break;

    // set # of rounds
    case STATE_SETUP2:
      if (pot != lastVal) {
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

        displayRoundMax();
      }
      
      // continue condition
      if (posedge[0]) {
        state = STATE_ROUND;
        roundCur = 1;
        
        // give brief setup time
        lcd.clear();
        lcd.print("Get Ready...");
        delay(3000);
        
        displayState();
        buzzOnce();
        msStart = millis();
      }
      // return condition
      else if (posedge[1]) {
        state = STATE_SETUP1;
        displayState();
        pot = -1;
      }

      // update last pot value
      lastVal = pot;
      
      break;

    // active round
    case STATE_ROUND:
      updateTime(secRound);
      displayTime(secRemain);

      // rest condition
      if (secRemain <= 0) {
        if (roundCur == roundMax) {
          state = STATE_SETUP0;

          // signal end of activity
          lcd.clear();
          lcd.print("Complete!");
          buzzThrice();
          delay(2000);
          
          displayState();
          lastVal = -1;
        } else {
          state = STATE_REST;
          displayState();
          buzzThrice();
          msStart = millis();
        }
      }
      else if (posedge[1]) {
        state = STATE_ROUND_PAUSE;
        displayState();
      }
      
      break;

    // active rest period
    case STATE_REST:
      updateTime(secRest);
      displayTime(secRemain);
      
      // round condition
      if (secRemain <= 0) {
        state = STATE_ROUND;
        roundCur++;
        displayState();
        buzzOnce();
        msStart = millis();
      }
      else if (posedge[1]) {
        state = STATE_REST_PAUSE;
        displayState();
      }
      
      break;

    // round paused
    case STATE_ROUND_PAUSE:
      // resume condition
      if (posedge[0]) {
        state = STATE_ROUND;
        displayState();
        secRound = secRemain;
        msStart = millis();
      }
      // finish condition
      else if (posedge[1]) {
        state = STATE_SETUP0;
        displayState();
        lastVal = -1;
      }
      
      break;

    // rest period paused
    case STATE_REST_PAUSE:  
      // resume condition
      if (posedge[0]) {
        state = STATE_REST;
        displayState();
        secRest = secRemain;
        msStart = millis();
      }
      // finish condition
      else if (posedge[1]) {
        state = STATE_SETUP0;
        displayState();
        lastVal = -1;
      }
      
      break;
      
    default:
      /*
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
      */
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
// Tracks changes in input and time since last function call
// Input: an array containing two bool values
// Ouput: updates the input array to reflect posedges (no return value)
void updatePosedge(bool posedge[2]) {
  int butG = digitalRead(butGPin);
  int butR = digitalRead(butRPin);
  static int butGLast = LOW;
  static int butRLast = LOW;
  static unsigned long msGLast = 0;
  static unsigned long msRLast = 0;

  // if falling edge, update time of last fall
  if (butG == LOW && butGLast == HIGH) {
    msGLast = millis();
  }
  // if rising edge, check time since last fall
  else if (butG == HIGH && butGLast == LOW && (millis() - msGLast > msBounce)) {
    posedge[0] = true;
  }

  // if falling edge, update time of last fall
  if (butR == LOW && butRLast == HIGH) {
    msRLast = millis();
  }
  // if rising edge, check time since last fall
  else if (butR == HIGH && butRLast == LOW && (millis() - msRLast > msBounce)) {
    posedge[1] = true;
  }

  // update last button value
  butGLast = butG;
  butRLast = butR;
}

// Decrements global variable "secRemain" approximately every second
// Uses unsigned long to handle millis() rollover
// Input: duration of time period in seconds
void updateTime(int duration) { 
  unsigned long msCur = millis();
  secRemain = duration - ((msCur - msStart) / 1000);
}

// Displays time in M:SS format on second line of LCD 
// Input: time in seconds
void displayTime(int sec) {
  lcd.setCursor(0, 1);
  lcd.print(sec / 60);
  lcd.print(":");
  int mod = sec % 60;
  if (mod < 10) 
    lcd.print("0");
  lcd.print(mod);
}

// Displays max round on second line of LCD
void displayRoundMax() {
  // erase old round (may be a difference in digits)
  lcd.setCursor(0, 1);
  lcd.print("  ");
  // write new round
  lcd.setCursor(0, 1);
  lcd.print(roundMax);
}

/* 
 * Displays the appropriate message on the LCD 
 * based on the current state (if appropriate)
 * 
 * NOTE: successive calls to lcd.print() is faster
 * & more efficient than concatenating multiple strings
 * 
 * Previously used buffers to save lines of code, but
 * ran into memory issues
 */
void displayState() {
  lcd.clear();
  
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
      lcd.print("Round ");
      lcd.print(roundCur);
      lcd.print(" / ");
      lcd.print(roundMax);
      break;
      
    case STATE_REST:
      lcd.print("Rest ");
      lcd.print(roundCur);
      lcd.print(" -> ");
      lcd.print(roundCur + 1);
      lcd.print("/");
      lcd.print(roundMax);
      break;
      
    case STATE_ROUND_PAUSE:
      lcd.print("Round ");
      lcd.print(roundCur);
      lcd.print(" / ");
      lcd.print(roundMax);
      
      displayTime(secRemain);
      
      lcd.setCursor(5, 1);
      lcd.print("PAUSED");
      break;
      
    case STATE_REST_PAUSE:
      lcd.print("Rest ");
      lcd.print(roundCur);
      lcd.print(" -> ");
      lcd.print(roundCur + 1);
      lcd.print("/");
      lcd.print(roundMax);
      
      displayTime(secRemain);
      
      lcd.setCursor(5, 1);
      lcd.print("PAUSED");
      break;
      
    default:
      break;
  }
}

// plays a single long buzz
// NOTE: uses delay()
void buzzOnce() {
  tone(buzPin, 1000);
  delay(1000);
  noTone(buzPin);
}

// plays a three short buzzes
// NOTE: uses delay()
void buzzThrice() {
  for (int i = 0; i < 3; i++) {
    tone(buzPin, 1000);
    delay(150);
    noTone(buzPin);
    delay(50);
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
