//**************************************************************//
//  Name    : shiftOutCode, 4 Digit 7 segment display
//  Author  : Edgar Solis Vizcarra
//  Date    : 29 Mar, 2017
//  Modified: 20 Apr, 2017
//  Version : 1.3
//  Notes   : Code for controlling a 2 channel solid state relay
//          : with a user defined variable time on each channel. 
//          : it is activated with a signal obtained from a sensor
//          : and the time each is on is shown in a 3 digit display
//          : with the two displays being controlled with 8 bit shift
//          : registers daisy chained.
//          : Branch without screen readers.
//****************************************************************

#include <TimedAction.h>
  


////Pin connected to IN1 of SSR
int relay1 = 5;
////Pin connected to IN2 of SSR
int relay2 = 6;
////Pin connected to button
int buttonPin = 2;     // the number of the pushbutton pin
////Pin connected to sensor
int sensorPin = 3;     // the number of the sensor pin
////Pin connected to sensor
int currentPin = 7;

////VARIABLES
int sp1 = 0;
int sp2 = 0;
int sensorState = LOW;
unsigned long previousMillis = 0;

////CONSTANTS
int val1 = 300;
int val2 = 300;
long onTime = 300;
int buttonState = 0;         // variable for reading the pushbutton status



void setup() {
  //set pins to output so you can control the shift register
//  pinMode(latchPin, OUTPUT);
//  pinMode(clockPin, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
//  pinMode(dataPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(sensorPin, INPUT);
  pinMode(currentPin, INPUT);
  digitalWrite(currentPin, HIGH);
}

////FUNCTIONS
  TimedAction buttonThread = TimedAction(100,buttonStatus);

void buttonStatus (){
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    sp1 ++;
  }
}

void loop() {
  sensorState = digitalRead(sensorPin);
  unsigned long currentMillis = millis();
  if (sensorState == HIGH){
    previousMillis = currentMillis;
    digitalWrite(relay1, HIGH);
  }
  if ((sensorState == LOW) && (currentMillis - previousMillis >= ((onTime-sp1) * 10))){
    previousMillis = currentMillis;
    digitalWrite(relay1, LOW);
  }
  buttonThread.check();
}
