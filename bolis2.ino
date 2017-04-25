//**************************************************************//
//  Name    : shiftOutCode, 4 Digit 7 segment display
//  Author  : Edgar Solis Vizcarra
//  Date    : 29 Mar, 2017
//  Modified: 25 Apr, 2017
//  Version : 1.4
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



void setup() {
  //set pins to output so you can control the shift register
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(button4Pin, INPUT);
  pinMode(sensorPin, INPUT);
  pinMode(pausePin, INPUT);
  Serial.begin(2000000);
}

////FUNCTIONS
  TimedAction buttonThread = TimedAction(100,buttonStatus); //This controls how often we check for input from the buttons first value is in ms second is callback function

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
  Serial.print(onTime-sp1);
  Serial.print(" v1");
  Serial.print("\n");
  Serial.print(onTime-sp2);
  Serial.print(" v2");
  Serial.print("\n");
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
    buttonThread.check();
  }
  }
