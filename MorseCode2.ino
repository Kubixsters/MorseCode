#include "Timer.h"
#include "Words.h"
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum LCD_States1 {LCD_Write, LCD_Practice, LCD_Letters, LCD_Buffer, LCD_LettPrac} LCD_State1;
enum LED_States1 {LED_Off, LED_Buffer, LED_Dot, LED_Dash, LED_Beat1, LED_Beat2, LED_Beat3, LED_Beat4, LED_Repeat} LED_State1;

char buttonEncoder = 0;
char buttonPush = 0;
int prevLCDState;
int prevLEDState;
int passBuzz = 22;

int unit = 0;
long randomNumber;
int easyPick;
int mediumPick;
int hardPick;
int challengePick;

char* easyWord;
char* mediumWord;
char* hardWord;
char* challengeWord;

int counter = 0;
char ch = 'A';
bool FlagLett = false;
bool FlagRepeat = false;
int lastStateCLK;
int currStateCLK;

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
      if (buttonPush) {
        lcd.clear();
        LCD_State1 = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State1 = LCD_Practice;
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
        LCD_State1 = LCD_Buffer;
      }
      else if (!buttonPush && prevLCDState == LCD_Write) {
        LCD_State1 = LCD_Practice;
      }
      else if (!buttonPush && prevLCDState == LCD_Practice) {
        LCD_State1 = LCD_Letters;
      }
      else if ( (!buttonPush && !FlagLett) && prevLCDState == LCD_Letters) {
        LCD_State1 = LCD_Write;
      }
      else if ( (buttonEncoder && FlagLett) && prevLCDState == LCD_Letters) {
        LCD_State1 = LCD_LettPrac;
      }
      else if (!buttonPush && prevLCDState == LCD_LettPrac) { /// added this 6/30
        LCD_State1 = LCD_LettPrac;
      }
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
      else if (ch == 'F' || ch == 'H' || ch == 'J' || ch == 'L' || ch == 'P' || ch == 'V') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'B' || ch == 'C' || ch == 'Q' || ch == 'X' || ch == 'Y' || ch == 'Z') {
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
      else if (ch == 'B' || ch == 'C' || ch == 'F' || ch == 'H' || ch == 'V' || ch == 'X' || ch == 'Y') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'J' || ch == 'L' || ch == 'P' || ch == 'Q' || ch == 'Z') {
        LED_State1 = LED_Dash;
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
      else if (ch == 'B' ||  ch == 'H' || ch == 'L' || ch == 'Q' || ch == 'V' || ch == 'X' || ch == 'Z') {
        LED_State1 = LED_Dot;
      }
      else if (ch == 'C' || ch == 'F' || ch == 'J' || ch == 'P' || ch == 'Y') {
        LED_State1 = LED_Dash;
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
  tick_LED();
  
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
