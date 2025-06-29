
#include "Timer.h"
#include "Words.h"
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

enum LCD_States {LCD_Start, LCD_Write, LCD_Practice, LCD_Easy, LCD_Medium, LCD_Hard, LCD_Challenge, LCD_Letters, LCD_Buffer, LCD_LettPrac} LCD_State;

char buttonEncoder = 0;
char buttonPush = 0;
// char myWords[154];
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
char ch = 'a';
int lastStateCLK;
int currStateCLK;

int redPin = 41;
int greenPin = 40;
int bluePin = 38;

void tick_LCD() {
  
  // transistions
  switch (LCD_State) {
    case LCD_Start:
      LCD_State = LCD_Write;
      prevLCDState = LCD_Start;
      break;

    case LCD_Write:
      prevLCDState = LCD_Write;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State = LCD_Write;
      }
      break;
    
    case LCD_Practice:
      prevLCDState = LCD_Practice;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State = LCD_Practice;
        
        if (!buttonEncoder) {
          lcd.clear();
          LCD_State = LCD_Easy;
        }
      }
      break;

    case LCD_Letters:
      prevLCDState = LCD_Letters;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State = LCD_Letters;

        if (!buttonEncoder) {
          lcd.clear();
          LCD_State = LCD_LettPrac;
        }
      }
      break;

    case LCD_Easy:
      prevLCDState = LCD_Easy;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State = LCD_Easy;
      }
      break;
    
    case LCD_Medium:
      prevLCDState = LCD_Medium;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        lcd.clear();
        LCD_State = LCD_Medium;
      }
      break;

    case LCD_Hard:
      prevLCDState = LCD_Hard;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State = LCD_Hard;
      }
      break;

    case LCD_Challenge:
      prevLCDState = LCD_Challenge;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush) {
        LCD_State = LCD_Challenge;
      }
      break;

    case LCD_LettPrac:
      prevLCDState = LCD_LettPrac;
      if (buttonPush) {
        lcd.clear();
        LCD_State = LCD_Buffer;
      } 
      else if (!buttonPush) {
        LCD_State = LCD_LettPrac;
      }
      break;
    
    case LCD_Buffer:
      if (buttonPush) {
        LCD_State = LCD_Buffer;
      }
      else if (!buttonPush && prevLCDState == LCD_Write) {
        LCD_State = LCD_Practice;
      }
      else if (!buttonPush && prevLCDState == LCD_Practice) {
        LCD_State = LCD_Letters;
      }
      else if (!buttonPush && prevLCDState == LCD_Letters) {
        LCD_State = LCD_Start;
      }
      else if (!buttonPush && prevLCDState == LCD_Easy) {
        LCD_State = LCD_Medium;
      }
      else if (!buttonPush && prevLCDState == LCD_Medium) {
        LCD_State = LCD_Hard;
      }
      else if (!buttonPush && prevLCDState == LCD_Hard) {
        LCD_State = LCD_Challenge;
      }
      else if (!buttonPush && prevLCDState == LCD_Challenge) {
        LCD_State = LCD_Easy;
      }
      else if (!buttonPush && prevLCDState == 111) {
        LCD_State = LCD_LettPrac;
      }
      else if (!buttonPush && prevLCDState == 111) {
        // LCD_State = LCD_
      }
      else if (!buttonPush && prevLCDState == 111) {
        // LCD_State = LCD_
      }

      break;
  }

  // actions
  switch (LCD_State) {
    case LCD_Start:
      break;
    
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

    case LCD_Easy:
      // TimerOff();
      easyPick = random(52);
      easyWord = myWords[easyPick];
      lcd.setCursor(3, 0);
      lcd.write("Easy  Mode");
      lcd.setCursor(6, 1);
      lcd.write(myWords[easyPick]);
      break;
    
    case LCD_Medium:
      // TimerOff();
      mediumPick = random(52, 104);
      mediumWord = myWords[mediumPick];
      lcd.setCursor(2, 0); 
      lcd.write("Medium  Mode");
      lcd.setCursor(0, 1); // maybe set a case where to try to center the word in lcd
      lcd.write(myWords[mediumPick]);
      break;

    case LCD_Hard:
      // TimerOff();
      hardPick = random(104, 156);
      hardWord = myWords[hardPick];
      lcd.setCursor(3, 0); // 0000000000000000
      lcd.write("Hard  Mode"); // 
      lcd.setCursor(0, 1); // maybe set a case where to try to center the word in lcd
      lcd.write(myWords[hardPick]);
      break;

    case LCD_Challenge:
      // TimerOff();
      // challengePick = random();
      // challengeWord = myWords[challengePick]
      lcd.setCursor(1, 0); // 0000000000000000
      lcd.write("Challenge Mode");
      lcd.setCursor(0, 1); // maybe set a case where to try to center the word in lcd
      // lcd.write(myWords[challengeWord]);
      break;

    case LCD_LettPrac:
      lcd.setCursor(3, 0); // 0000000000000000
      lcd.write("Hard  Mode"); // 
      lcd.setCursor(2, 1); // (7, 0)
      lcd.write("practicing"); // (ch)
      break;

    case LCD_Buffer:
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

  // tone(passBuzz, 440);

  // int easy = random(52);
  // Serial.println(myWords[easy]);
  // char* easyWord = myWords[easy];
 
  // for (int i = 0; easyWord[i] != '\0'; i++) {
  //   Serial.println(easyWord[i]);
  // }

  //currStateCLK = digitalRead(50);
  //  if (currStateCLK != lastStateCLK && currStateCLK == HIGH) {
  //    if (digitalRead(51) != HIGH) {
  //      counter--;
  //      ch--;
//
  //    }
  //    else {
  //      counter++;
  //      ch++;
  //    }
  //    //Serial.println(counter);
  //    Serial.println(ch);
  //  }
  //  lastStateCLK = currStateCLK;

  

  while (!TimerFlag) {}
  TimerFlag = 0;
}

void soundLetters() {
  i = 0;
  if ( !(i < 1000)) {
    tone(passBuzz, 440);
    i = 0;
  }
  if ( !(i < 1000)) {
    noTone(passBuzz);
  }
  if ( !(i < 3000)) {
    tone(passBuzz, 440);
  }

}



// displays one number (between 0 and 9) "targetNum" on the digit conneceted to "digitPin"
// E.g. If I wanted to display the number 6 on the third digit of my display.
// and the third digit was connected to pin A0, then I'd write: displayNumTo7Seg(6, A0);
// void displayNumTo7Seg(unsigned int targetNum, int digitPin) {
// 
//     // A map of integers to the respective values needed to display
//     // a value on one 7 seg digit.
//     unsigned char encodeInt[10] = {
//         // 0     1     2     3     4     5     6     7     8     9
//         0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
//     };
// 
//     // Make sure the target digit is off while updating the segments iteratively
//     digitalWrite(digitPin, HIGH);
// 
//     // Update the segments
//     for (int k = 0; k < 7; ++k) {
//        // digitalWrite(gSegPins[k], encodeInt[targetNum] & (1 << k));
//     }
// 
//     // Turn on the digit again
//     digitalWrite(digitPin, LOW);
// }

// currStateCLK = digitalRead(50);
//   if (currStateCLK != lastStateCLK && currStateCLK == 1) {
//     if (digitalRead(51) != currStateCLK) {
//       counter--;
//     }
//     else {
//       counter++;
//     }
//     Serial.println(counter);
//   }
//   lastStateCLK = currStateCLK;

// easyWord = random(52);
  // mediumWord = random(52, 104);
  // hardWord = random(104, 156);
  // challengeWord = random();

  // Serial.print(easyWord);
  // Serial.print(" ");
  // Serial.print(myWords[easyWord]);
  // Serial.print("\n");

  // Serial.print(mediumWord);
  // Serial.print(" ");
  // Serial.print(myWords[mediumWord]);
  // Serial.print("\n");

  // Serial.print(hardWord);
  // Serial.print(" ");
  // Serial.print(myWords[hardWord]);
  // Serial.print("\n");

  //if (!buttonPush) {
  //  Serial.print("not pushed");
  //  Serial.print("\n");
  //}
  //else if (buttonPush) {
  //  Serial.print("pushed");
  //  Serial.print("\n");
  //}

  //if (buttonEncoder) {
  //  Serial.print("not clicked");
  //  Serial.print("\n");
  //}
  //else if (!buttonEncoder) {
  //  Serial.print("clicked");
  //  Serial.print("\n");
  //}

  // tone(passBuzz, 440); // A4
  // delay(1000);
  // tone(passBuzz, 494); // B4
  // delay(1000);
  // tone(passBuzz, 523); // C4
  // delay(1000);
  // tone(passBuzz, 587); // D4
  // delay(1000);
  // tone(passBuzz, 659); // E4
  // delay(1000);
  // tone(passBuzz, 698); // F4
  // delay(1000);
  // tone(passBuzz, 784); // G4
  // delay(1000);
  // noTone(passBuzz);
  // delay(1000);

// char a = 43;
// char b = 42;
// char c = 31;
// char d = 32;
// char e = 33;
// char f = 44;
// char g = 45;

// pinMode(a, OUTPUT); // 1 Digit 7 Segment Display
// pinMode(b, OUTPUT);
// pinMode(c, OUTPUT);
// pinMode(d, OUTPUT);
// pinMode(e, OUTPUT);
// pinMode(f, OUTPUT);
// pinMode(g, OUTPUT);

// digitalWrite(a, LOW); // segment ; Is powered/enabled/on when HIGH
// digitalWrite(b, LOW);
// digitalWrite(c, LOW);
// digitalWrite(d, LOW);
// digitalWrite(e, LOW);
// digitalWrite(f, LOW);
// digitalWrite(g, LOW);

  // Off (all LEDs off):

  // digitalWrite(redPin, LOW);
  // digitalWrite(greenPin, LOW);
  // digitalWrite(bluePin, LOW);

  // delay(1000);

  // Red (turn just the red LED on):

  // digitalWrite(redPin, HIGH);
  // digitalWrite(greenPin, LOW);
  // digitalWrite(bluePin, LOW);

  // delay(1000);

  // Green (turn just the green LED on):

  // digitalWrite(redPin, LOW);
  // digitalWrite(greenPin, HIGH);
  // digitalWrite(bluePin, LOW);

  // delay(1000);

  // Blue (turn just the blue LED on):

  // digitalWrite(redPin, LOW);
  // digitalWrite(greenPin, LOW);
  // digitalWrite(bluePin, HIGH);

  // delay(1000);

  // Yellow (turn red and green on):

  // digitalWrite(redPin, HIGH);
  // digitalWrite(greenPin, HIGH);
  // digitalWrite(bluePin, LOW);

  // delay(1000);

  // Cyan (turn green and blue on):

  // digitalWrite(redPin, LOW);
  // digitalWrite(greenPin, HIGH);
  // digitalWrite(bluePin, HIGH);

  // delay(1000);

  // Purple (turn red and blue on):

  // digitalWrite(redPin, HIGH);
  // digitalWrite(greenPin, LOW);
  // digitalWrite(bluePin, HIGH);

  // delay(1000);

  // White (turn all the LEDs on):

  // digitalWrite(redPin, HIGH);
  // digitalWrite(greenPin, HIGH);
  // digitalWrite(bluePin, HIGH);

  // delay(1000);