//**************************************************************//
//  Name    : shiftOutCode, 4 Digit 7 segment display
//  Author  : Edgar Solis Vizcarra
//  Date    : 29 Mar, 2017
//  Modified: 17 Apr, 2017
//  Version : 1.2
//  Notes   : Code for controlling a 2 channel solid state relay
//          : with a user defined variable time on each channel. 
//          : it is activated with a signal obtained from a sensor
//          : and the time each is on is shown in a 3 digit display
//          : with the two displays being controlled with 8 bit shift
//          : registers daisy chained.
//****************************************************************

#include <TimedAction.h>
  


//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;
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
int digits1[3];
int digits2[3];
int sensorState = LOW;
unsigned long previousMillis = 0;

////CONSTANTS
int val1 = 300;
int val2 = 300;
long onTime = 300;
int buttonState = 0;         // variable for reading the pushbutton status

////BYTES TEMPLATES FOR SHIFTOUT
byte digz[4] = {
  B10010000,
  B01001000,
  B00100100,
  B00000000
};

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


void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(sensorPin, INPUT);
  pinMode(currentPin, INPUT);
  digitalWrite(currentPin, HIGH);
}

////FUNCTIONS
  TimedAction displayThread = TimedAction(10,refresh);
  TimedAction buttonThread = TimedAction(100,buttonStatus);

void refresh(){
  val1 = 300;
  val2 = 300;
  val1 = val1 - sp1;
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
    //delay(1);
    shiftOut(dataPin, clockPin, LSBFIRST, number2);
    delay(2);
    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
    //delay(1); //you can play with this delay to watch how it loops. try 100 instead of 4
 }
 delay(4);
}

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
  displayThread.check();
  buttonThread.check();
}
