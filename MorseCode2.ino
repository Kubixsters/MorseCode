#include "Timer.h"
#include "Words.h"
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum LCD_States1 {LCD_Write, LCD_Practice, LCD_PractIn, LCD_Letters, LCD_LettPrac, LCD_Buffer} LCD_State1;
enum LED_States1 {LED_Off, LED_Buffer, LED_Dot, LED_Dash, LED_Beat1, LED_Beat2, LED_Beat3, LED_Beat4, LED_Repeat} LED_State1;
enum DifficultyStates {Easy, Medium, Hard, Challenge, DiffBuffer} Difficulty_State;

char buttonEncoder = 0;
char buttonPush = 0;
int passBuzz = 22;
int unit = 0;
int counter = 0;

int prevLCDState;
int prevLEDState;
int prevDiffState;

long randomNumber;
int easyPick;
int mediumPick;
int hardPick;
int challengePick;

char* easyWord;
char* mediumWord;
char* hardWord;
char* challengeWord;

// easyWord = myWords[random(52)];

char ch = 'A';
int lastStateCLK;
int currStateCLK;

bool FlagLett = false;
bool FlagRepeat = false;
bool FlagPract = false;
bool isEasy = false;
bool isMedium = false;
bool isHard = false;
bool isChallenge = false;

int redPin = 41;
int greenPin = 40;
int bluePin = 38;

void tick_LCD() {

  // transitions
  switch (LCD_State1) {
    case LCD_Write:
      prevLCDState = LCD_Write;
      if (buttonPush) {
        lcd.clear();
        LCD_State1 = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State1 = LCD_Write;
      }
      break;

    case LCD_Practice:
      prevLCDState = LCD_Practice;
      FlagPract = false;
      if (buttonPush) {
        lcd.clear();
        LCD_State1 = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State1 = LCD_Practice;

        if (!buttonEncoder) {
          Serial.println("pushed buttonENC in Practice");
          lcd.clear();
          TimerSet(200);
          FlagPract = true;
          easyWord = myWords[random(52)]; /// yoooooooooo
          mediumWord = myWords[random(52, 104)];
          hardWord = myWords[random(104, 156)];
          challengeWord = myWords[random(156, 182)];
          LCD_State1 = LCD_Buffer;
        }
      }
      break;

    case LCD_PractIn:
      if (!buttonEncoder) {
        lcd.clear();
        TimerSet(1);
        FlagPract = false;
        Difficulty_State = Easy;
        LCD_State1 = LCD_Write;
      }
      else if (buttonEncoder) {
        LCD_State1 = LCD_PractIn; /// might not have to make it loop itself
      }
      break;
    
    case LCD_Letters:
      prevLCDState = LCD_Letters;
      FlagLett = false;
      if (buttonPush) { // pushed == 1;;; 0 default
        lcd.clear();
        LCD_State1 = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State1 = LCD_Letters;

        if (!buttonEncoder) { // pushed == 0;;; 1 default
          lcd.clear();
          TimerSet(200);
          FlagLett = true;
          LCD_State1 = LCD_Buffer;
        }
      }
      break;

    case LCD_LettPrac:
      if (!buttonEncoder) {
        lcd.clear();
        TimerSet(1);
        FlagLett = false;
        ch = 'A';
        LED_State1 = LED_Off;
        LCD_State1 = LCD_Write;
      }
      else if (buttonEncoder) {
        LCD_State1 = LCD_LettPrac;

        if (buttonPush) { /// added this 6/30
          Serial.println("changed char");
          if (ch >= 'Z') {
            ch = 'A';
          }
          else if (ch < 'Z') {
            ++ch;
          }
        }
      }
      break;

    case LCD_Buffer:
      if (buttonPush) {
        Serial.println("Buffer");
        LCD_State1 = LCD_Buffer;
      }
      else if (!buttonPush && prevLCDState == LCD_Write) {
        Serial.println("Buffer LCDPractice");
        LCD_State1 = LCD_Practice;
      }
        else if ( (!buttonEncoder && FlagPract) && prevLCDState == LCD_Practice) {
        Serial.println("Buffer LCDPractIn");
        LCD_State1 = LCD_PractIn;
      }
      else if (!buttonPush && prevLCDState == LCD_Practice) {
        Serial.println("Buffer LCDLetters");
        LCD_State1 = LCD_Letters;
      }
      else if ( (!buttonPush && !FlagLett) && prevLCDState == LCD_Letters) {
        Serial.println("Buffe LCDWriter");
        LCD_State1 = LCD_Write;
      }
      else if ( (buttonEncoder && FlagLett) && prevLCDState == LCD_Letters) {
        Serial.println("Buffer LCDLettPrac1");
        LCD_State1 = LCD_LettPrac;
      }
      else if (!buttonPush && prevLCDState == LCD_LettPrac) { /// added this 6/30
        Serial.println("Buffer LCDLettPrac2");
        LCD_State1 = LCD_LettPrac;
      }
      break;
  }

  // actions
  switch (LCD_State1) {
    case LCD_Write:
      lcd.setCursor(6, 0);
      lcd.write("Write");
      lcd.setCursor(3, 1);
      lcd.write("Morse Code");
      LED_Blank();
      break;
    
    case LCD_Practice:
      lcd.setCursor(4, 0);
      lcd.write("Practice");
      lcd.setCursor(3, 1);
      lcd.write("Morse Code");
      break;

    case LCD_PractIn:
      break;

    case LCD_Letters:
      lcd.setCursor(3, 0);
      lcd.write("Letters of");
      lcd.setCursor(3, 1);
      lcd.write("Morse Code");
      break;

    case LCD_LettPrac:
      lcd.setCursor(5, 0);
      lcd.write("Letter:");
      lcd.setCursor(8, 1);
      lcd.write(ch);
      break;

    case LCD_Buffer:
      break;
  }
}

void tick_LED() {

  // transitions
  switch (LED_State1) {
    case LED_Off:
      Serial.println("off");
      if (!FlagLett) {
        LED_State1 = LED_Off;
      }
      else if (FlagLett) {
        LED_State1 = LED_Beat1;
      }
      break;

    case LED_Buffer:
      Serial.println("buffer");
      if (buttonPush) { ////added this july 2
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (unit < 1) {
        LED_State1 = LED_Buffer;
      }
      else if ( !(unit < 1) ) {
        if (prevLEDState == LED_Beat1) {
          LED_State1 = LED_Beat2;
        }
        else if (prevLEDState == LED_Beat2) {
          LED_State1 = LED_Beat3;
        }
        else if (prevLEDState == LED_Beat3) {
          LED_State1 = LED_Beat4;
        }
        else if (prevLEDState == LED_Beat4) {
          LED_State1 = LED_Beat1;
        }
      }
      break;

    case LED_Dot:
      Serial.println("dot");
      if (buttonPush) { ////added this july 2
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (unit < 1) {
        LED_State1 = LED_Dot;
      }
      else if  ( !(unit < 1) ) {
        unit = 0;
        LED_State1 = LED_Buffer;
      }
      break;

    case LED_Dash:
      Serial.println("dash");
      if (buttonPush) { ////added this july 2
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (unit < 5) {
        LED_State1 = LED_Dash;
      }
      else if ( !(unit < 5) ) {
        unit = 0;
        LED_State1 = LED_Buffer;
      }
      break; 
    
    case LED_Beat1:
      prevLEDState = LED_Beat1;
      Serial.println("beat1");
      if (buttonPush) { ////added this july 2
        Serial.println("beat1 pB");
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (ch == 'A' || ch == 'E' || ch == 'F' || ch == 'H' || ch == 'I' || ch == 'J' || ch == 'L' || ch == 'P' || ch == 'R' || ch == 'S' || ch == 'U' || ch == 'V' || ch == 'W') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'B' || ch == 'C' || ch == 'D' || ch == 'G' || ch == 'K' || ch =='M' || ch == 'N' || ch == 'O' || ch == 'Q' || ch == 'T' || ch == 'X' || ch == 'Y' || ch == 'Z') {
        LED_State1 = LED_Dash;
      }
      break;
    
    case LED_Beat2:
      prevLEDState = LED_Beat2;
      Serial.println("beat2");
      if (buttonPush) { ////added this july 2
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (ch == 'B' || ch == 'C' || ch == 'D' || ch == 'F' || ch == 'H' || ch == 'I' || ch == 'K' || ch == 'N' || ch == 'S' || ch == 'U' || ch == 'V' || ch == 'X' || ch == 'Y') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'A' || ch == 'G' || ch == 'J' || ch == 'L' || ch == 'M' || ch == 'O' || ch == 'P' || ch == 'Q' || ch == 'R' || ch == 'W' || ch == 'Z') {
        LED_State1 = LED_Dash;
      }
      else if (ch == 'E' || ch == 'T') {
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      break;

    case LED_Beat3:
      prevLEDState = LED_Beat3;
      Serial.println("beat3");
      if (buttonPush) { ////added this july 2
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (ch == 'B' || ch == 'D' || ch == 'G' || ch == 'H' || ch == 'L' || ch == 'Q' || ch == 'R' || ch == 'S' || ch == 'V' || ch == 'X' || ch == 'Z') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'C' || ch == 'F' || ch == 'J' || ch == 'K' || ch == 'O' || ch == 'P' || ch == 'U' || ch == 'W' || ch == 'Y') {
        LED_State1 = LED_Dash;
      }
      else if (ch == 'A' || ch == 'I' || ch == 'M' || ch == 'N') {
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      break;

    case LED_Beat4:
      prevLEDState = LED_Beat4;
      Serial.println("beat4");
      if (buttonPush) { ////added this july 2
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (ch == 'B' || ch == 'C' || ch == 'F' || ch == 'H' || ch == 'L' || ch == 'P' || ch == 'Z') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'J' || ch == 'Q' || ch == 'V' || ch == 'X' || ch == 'Y') {
        LED_State1 = LED_Dash;
      }
      else if (ch == 'D' || ch == 'G' || ch == 'K' || ch == 'O' || ch == 'R' || ch == 'S' || ch == 'U' || ch == 'W') {
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      break;

    case LED_Repeat:
      Serial.println("repeat"); // write the condition for when bP during this case
      if (buttonPush) {
        unit = 0;
        LED_State1 = LED_Repeat;
        prevLEDState = LED_Beat1;
      }
      else if (unit < 15) {
        LED_State1 = LED_Repeat;
      }
      else if ( !(unit < 15) ) {
        unit = 0;
        LED_State1 = LED_Off;
      }
      break;
  }

  // actions
  switch (LED_State1) {
    case LED_Off:
      unit = 0;
      LED_Blank();
      break;

    case LED_Buffer:
      ++unit;
      LED_Blank();
      break;

    case LED_Dot:
      ++unit;
      LED_On();
      break;

    case LED_Dash:
      ++unit;
      LED_On();
      break;

    case LED_Beat1:
      break;
    
    case LED_Beat2:
      break;

    case LED_Beat3:
      break;

    case LED_Beat4:
      break;

    case LED_Repeat:
      ++unit;
      LED_Blank();
      break;
  }
}

void tick_Diff() {
   
   // transitions
   switch (Difficulty_State) {
      case Easy:
        prevDiffState = Easy;
        if (buttonPush) {
          lcd.clear();
          Difficulty_State = DiffBuffer;
        }
        else if (!buttonPush) {
          Difficulty_State = Easy;
        }
        break;
      
      case Medium: 
        prevDiffState = Medium;
        if (buttonPush) {
          lcd.clear();
          Difficulty_State = DiffBuffer;
        }
        else if (!buttonPush) {
          Difficulty_State = Medium;
        }
        break;

      case Hard:
        prevDiffState = Hard;
        if (buttonPush) {
          lcd.clear();
          Difficulty_State = DiffBuffer;
        }
        else if (!buttonPush) {
          Difficulty_State = Hard;
        }
        break;

      case Challenge:
        prevDiffState = Challenge;
        if (buttonPush) {
          lcd.clear();
          Difficulty_State = DiffBuffer;
        }
        else if (!buttonPush) {
          Difficulty_State = Challenge;
        }
        break;

      case DiffBuffer:
        if (buttonPush) {
          Difficulty_State = DiffBuffer;
        }
        else if (!buttonPush && prevDiffState == Easy) {
          Difficulty_State = Medium;
        }
        else if (!buttonPush && prevDiffState == Medium) {
          Difficulty_State = Hard;
        }
        else if (!buttonPush && prevDiffState == Hard) {
          Difficulty_State = Challenge;
        }
        else if (!buttonPush && prevDiffState == Challenge) {
          Difficulty_State = Easy;
        }
        break;
   }

   // actions
   switch (Difficulty_State) {
    case Easy:
      lcd.clear();
      isEasy = true;
      isMedium = false;
      isHard = false;
      isChallenge = false;
      lcd.setCursor(3, 0);
      lcd.write("Easy  Mode");
      lcd.setCursor(6, 1);
      lcd.write(easyWord);
      break;

    case Medium:
      lcd.clear();
      isEasy = false;
      isMedium = true;
      isHard = false;
      isChallenge = false;
      lcd.setCursor(2, 0); 
      lcd.write("Medium  Mode");
      lcd.setCursor(5, 1); // maybe set a case where to try to center the word in lcd
      lcd.write(mediumWord);
      break;

    case Hard:
      lcd.clear();
      isEasy = false;
      isMedium = false;
      isHard = true;
      isChallenge = false;
      lcd.setCursor(3, 0); // 0000000000000000
      lcd.write("Hard  Mode"); // 
      lcd.setCursor(3, 1); // maybe set a case where to try to center the word in lcd
      lcd.write(hardWord);
      break;

    case Challenge:
      lcd.clear();
      isEasy = false;
      isMedium = false;
      isHard = false;
      isChallenge = true;
      lcd.setCursor(1, 0); // 0000000000000000
      lcd.write("Challenge Mode");
      lcd.setCursor(2, 1); // maybe set a case where to try to center the word in lcd
      lcd.write(challengeWord);
      break;

    case DiffBuffer:
      break;
   }
}

void setup() {
  // put your setup code here, to run once:

  TimerSet(1);
  TimerOn();
  lcd.begin(16, 2);

  pinMode(22, OUTPUT); // passive buzzer
  pinMode(23, INPUT); // normal button
  pinMode(53, INPUT); // button on encoder
  pinMode(50, INPUT); // clk pin
  pinMode(51, INPUT); // dt pin

  pinMode(41, OUTPUT); // red
  pinMode(40, OUTPUT); // green
  pinMode(38, OUTPUT); // blue

  digitalWrite(53, HIGH);

  lastStateCLK = digitalRead(50);
  randomSeed(analogRead(A0));

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonPush = digitalRead(23);
  buttonEncoder = digitalRead(53);

  tick_LCD();

  if (FlagLett) {
    tick_LED();
  }

  if (FlagPract) {
    tick_Diff();
  }
  
  while (!TimerFlag) {}
  TimerFlag = 0;
}

void LED_On() {
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

void LED_Blank() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}
