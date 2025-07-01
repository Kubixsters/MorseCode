#include "Timer.h"
#include "Words.h"
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum LCD_States1 {LCD_Write, LCD_Practice, LCD_Letters, LCD_Buffer, LCD_LettPrac} LCD_State1;
enum LED_States1 {LED_Off, LED_On} LED_State1;

char buttonEncoder = 0;
char buttonPush = 0;
int prevLCDState;
int passBuzz = 22;

int i = 0;
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
      if (!FlagLett) {
        LED_State1 = LED_Off;
      }
      else if (FlagLett) {
        LED_State1 = LED_On;
      }
      break;

    case LED_On:
      // if (ch == ' ') {
      //   LED_State1 = LED_Off;
      // }
      // else if ( !(ch == ' ') ) {
      //   LED_State1 = LED_On;
      // }
        if (!FlagLett) {
          LED_State1 = LED_Off;
        }
        else if (FlagLett) {
          LED_State1 = LED_On;
        }
      break;
  }

  // actions
  switch (LED_State1) {
    case LED_Off:
      LED_Blank();
      break;

    case LED_On:
      i = 0;
      if (ch == 'A') {
        if (i < 1) {
          LED_Dot();
          ++i;
        }
        else if (i < 2) {
          LED_Blank();
          ++i;
        }
        else if (i < 5) {
          LED_Dash();
          ++i;
        }
      }
      if ( !(ch == 'A')) {
        LED_Blank();
      }
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

  // digitalWrite(redPin, HIGH);
  // digitalWrite(greenPin, HIGH);
  // digitalWrite(bluePin, HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonPush = digitalRead(23);
  buttonEncoder = digitalRead(53);

  tick_LCD();
  
  while (!TimerFlag) {}
  TimerFlag = 0;
}

void LEDLetters(char ch) {
  if (ch == 'A') {
    LED_Dot();
    LED_Blank();
    LED_Dash();
  }
  if (ch == 'B') {
    LED_Dash();
    LED_Blank();
    LED_Dot();
    LED_Blank();
    LED_Dot();
    LED_Blank();
    LED_Dot();
    LED_Blank();
  }
}

void LED_Dot() {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
}

void LED_Dash() {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
}

void LED_Blank() {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
}
