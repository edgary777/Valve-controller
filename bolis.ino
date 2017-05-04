//**************************************************************//
//  Name    : shiftOutCode, 4 Digit 7 segment display
//  Author  : Edgar Solis Vizcarra
//  Date    : 29 Mar, 2017
//  Modified: 26 Apr, 2017
//  Version : 2.0
//  Notes   : Code for controlling a 2 channel solid state relay
//          : with a user defined variable time on each channel. 
//          : it is activated with a signal obtained from a sensor
//          : and the time each is on is shown in a 3 digit display
//          : with the two displays being controlled with 3 8 bit shift
//          : registers daisy chained. 4 buttons control the time
//****************************************************************

#include <TimedAction.h>
  
////Pin connected to DS of 74HC595
int dataPin = 8;
//Pin connected to ST_CP of 74HC595
int latchPin = 9;
//Pin connected to SH_CP of 74HC595
int clockPin = 10;
////Pin connected to IN1 of SSR
int relay1 = 6;
////Pin connected to IN2 of SSR
int relay2 = 7;
////Pin connected to buttons
int button1Pin = 2;     // the number of the pushbutton pin
int button2Pin = 3;     // the number of the pushbutton pin
int button3Pin = 4;     // the number of the pushbutton pin
int button4Pin = 5;     // the number of the pushbutton pin
////Pin connected to sensor
int sensorPin = A1;
////Pin conected to the pause switch
int pausePin = A0;

////CONSTANTS
int baseTime = 300;                   // The time at which the code starts in ms
////BYTES TEMPLATES FOR SHIFTOUT
byte digz[4] = {
  B10010000,
  B01001000,
  B00100100,
  B00000000
};
////BYTES FOR THE NUMBERS
byte numbers[10] =
{
//ABCDEFG
 B00000011, // 0
 B10011111, // 1
 B00100101, // 2
 B00001101, // 3
 B10011001, // 4
 B01001001, // 5
 B01000001, // 6
 B00011111, // 7
 B00000001, // 8
 B00001001  // 9
};

////VARIABLES
int sp1 = 0;                          // The time to diminish the time the first relay is open
int sp2 = 0;                          // The time to diminish the time the second relay is open
int sensorState = LOW;                // Variable for reading the sensor status
unsigned long previousMillis = 0;     // The time for the counter for the first relay
unsigned long previousMillis2 = 0;    // The base for the counter for the second relay
long onTime = baseTime;               // The time in ms the relays are open
int pauseState = 0;                   // variable for reading the pause switch status
int button1State = 0;                 // variable for reading the pushbutton status
int button2State = 0;                 // variable for reading the pushbutton status
int button3State = 0;                 // variable for reading the pushbutton status
int button4State = 0;                 // variable for reading the pushbutton status
int digits1[3];
int digits2[3];


void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(button4Pin, INPUT);
  pinMode(sensorPin, INPUT);
  pinMode(pausePin, INPUT);
}

////FUNCTIONS
  TimedAction buttonThread = TimedAction(100,buttonStatus); //This controls how often we check for input from the buttons first value is in ms second is callback function
  TimedAction displayThread = TimedAction(10,refresh);

void refresh(){
  int val1 = onTime - sp1;
  int val2 = onTime - sp2;
  for (int i = 2; i >= 0; i--){ //We separate each of the elements of val and add it to our digits array
    digits1[i] = val1 % 10; // "% 10" gives us the rigthmost digit of our val int and assigns it to the appropiate value of our digits array
    val1 /= 10; // We get rid of the element we just added
    digits2[i] = val2 % 10; // "% 10" gives us the rigthmost digit of our val int and assigns it to the appropiate value of our digits array
    val2 /= 10; // We get rid of the element we just added
  }
  for (int h = 0; h < 3; h++){
    byte number1 = numbers[digits1[h]]; //We get the segments pattern from our binary patterns
    byte number2 = numbers[digits2[h]]; //We get the segments pattern from our binary patterns
    //delay(1);
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin, clockPin, LSBFIRST, digz[h]);
    shiftOut(dataPin, clockPin, LSBFIRST, number1);
    //delay(100);
    shiftOut(dataPin, clockPin, LSBFIRST, number2);
    delay(2);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
    //delay(1); //you can play with this delay to watch how it loops. try 100 instead of 4
 }
// delay(10);
}

void buttonStatus (){
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  button4State = digitalRead(button4Pin);
  pauseState = digitalRead(pausePin);
  if (button1State == LOW) {
    sp1 = sp1 - 5;
  }
  if ((button2State == LOW) && (sp1 < baseTime)) {
    sp1 =  sp1 + 5;
  }
  if (button3State == LOW) {
    sp2 = sp2 - 5;
  }
  if ((button4State == LOW)&& (sp2 < baseTime)) {
    sp2 = sp2 + 5;
  }
}

void loop() {
  sensorState = analogRead(sensorPin);  
  pauseState = analogRead(pausePin);  
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  unsigned long currentMillis = millis();
  unsigned long currentMillis2 = millis();
  if ((button1State == LOW) && (button2State == LOW)) {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
  } else if (pauseState < 100){
    if (sensorState < 850){
      previousMillis = currentMillis;
      digitalWrite(relay1, HIGH);
    }
    if ((sensorState > 850) && (currentMillis - previousMillis >= (onTime-sp1))){
      previousMillis = currentMillis;
        digitalWrite(relay1, LOW);
    }
    if (sensorState < 850){
      previousMillis2 = currentMillis2;
      digitalWrite(relay2, HIGH);
    }
    if ((sensorState > 850) && (currentMillis2 - previousMillis2 >= (onTime-sp2))){
      previousMillis2 = currentMillis2;
        digitalWrite(relay2, LOW);
    }
  } 
  if (pauseState > 100) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);    
  }
  buttonThread.check();
  displayThread.check();
}
