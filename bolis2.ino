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
int relay1 = 6;
////Pin connected to IN2 of SSR
int relay2 = 7;
////Pin connected to buttons
int button1Pin = 2;     // the number of the pushbutton pin
int button2Pin = 3;     // the number of the pushbutton pin
int button3Pin = 4;     // the number of the pushbutton pin
int button4Pin = 5;     // the number of the pushbutton pin
////Pin connected to sensor
int sensorPin = A1;     // the number of the sensor pin
int pausePin = A0;
////VARIABLES
int sp1 = 0;
int sp2 = 0;
int sensorState = LOW;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

////CONSTANTS
int baseTime = 300;
int val1 = baseTime;
int val2 = baseTime;
long onTime = baseTime;
int button1State = 0;         // variable for reading the pushbutton status
int button2State = 0;         // variable for reading the pushbutton status
int button3State = 0;         // variable for reading the pushbutton status
int button4State = 0;         // variable for reading the pushbutton status
int pauseState = 0;


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
  TimedAction buttonThread = TimedAction(100,buttonStatus);

void buttonStatus (){
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  button4State = digitalRead(button4Pin);
  pauseState = digitalRead(pausePin);
  if (button1State == LOW) {
    sp1 = sp1 - 5;
  }
  if (button2State == LOW) {
    sp1 =  sp1 + 5;
  }
  if (button3State == LOW) {
    sp2 = sp2 - 5;
  }
  if (button4State == LOW) {
    sp2 = sp2 + 5;
  }
}

void loop() {
  sensorState = analogRead(sensorPin);  
  pauseState = analogRead(pausePin);  
  Serial.println(sensorState);
//  Serial.print(onTime-sp1);
//  Serial.print(" v1");
//  Serial.print("\n");
//  Serial.print(onTime-sp2);
//  Serial.print(" v2");
//  Serial.print("\n");
//  Serial.println(onTime-sp1, "Valvula1");
//  Serial.println(onTime-sp2, "Valvula2");
  unsigned long currentMillis = millis();
  unsigned long currentMillis2 = millis();
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
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
