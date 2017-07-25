/*
 HC-SR04 Ping distance sensor]
 VCC to arduino 5v GND to arduino GND
 Echo to Arduino pin 13 Trig to Arduino pin 12
 Red POS to Arduino pin 11
 Green POS to Arduino pin 10
 560 ohm resistor to both LED NEG and GRD power rail
 More info at: http://goo.gl/kJ8Gl
 Original code improvements to the Ping sketch sourced from Trollmaker.com
 Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
 */
#include <SparkFun_RFD77402_Arduino_Library.h> //Use Library Manager or download here: https://github.com/sparkfun/SparkFun_RFD77402_Arduino_Library

RFD77402 myDistance; //Hook object to the library

#define ledBack 5
#define ledLeft 4
#define ledFront 2
#define ledRight 3

#define trigPinLeft 9
#define echoPinLeft 8
#define trigPinRight 13
#define echoPinRight 12
#define trigPinFront 11
#define echoPinFront 10

void setup() {
  Serial.begin (9600);
  while (!Serial);
  
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);
  pinMode(ledFront, OUTPUT);
  pinMode(ledBack, OUTPUT);
  
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);
  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);

  if (myDistance.begin() == false)
  {
    Serial.println("Qwiic Sensor failed to initialize. Check wiring.");
    while (1); //Freeze!
  }
  Serial.println("Qwiic Sensor online!");
}

void checkSensor(int trig, int eco, int led) {
  long duration, distance;
  digitalWrite(trig, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trig, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trig, LOW);
  duration = pulseIn(eco, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 40) {  
    digitalWrite(led, HIGH);
    Serial.println("danger on the ");
    Serial.println(trig);
  }
  else {
    digitalWrite(led, LOW);
  }
  if (distance >= 200 || distance <= 0){
    digitalWrite(led, LOW);
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  
}

void handleQwiic(int led)
{
  byte errorCode = myDistance.takeMeasurement();
  if (errorCode == CODE_VALID_DATA)
  {
    unsigned int distance = myDistance.getDistance();
    byte pixels = myDistance.getValidPixels();
    unsigned int confidence = myDistance.getConfidenceValue();

    Serial.print("distance: ");
    Serial.print(distance);
    Serial.print("mm pixels: ");
    Serial.print(pixels);
    Serial.print(" confidence: ");
    Serial.print(confidence);
    if (distance < 200)
    {
      digitalWrite(led, HIGH);
    }
    else
    {
      digitalWrite(led, LOW);
    }
    if(distance > 2000) Serial.print(" Nothing sensed");
  }
  else if (errorCode == CODE_FAILED_PIXELS)
  {
    Serial.print("Not enough pixels valid");
  }
  else if (errorCode == CODE_FAILED_SIGNAL)
  {
    Serial.print("Not enough signal");
  }
  else if (errorCode == CODE_FAILED_SATURATED)
  {
    Serial.print("Sensor pixels saturated");
  }
  else if (errorCode == CODE_FAILED_NOT_NEW)
  {
    Serial.print("New measurement failed");
  }
  else if (errorCode == CODE_FAILED_TIMEOUT)
  {
    Serial.print("Sensors timed out");
  }

  Serial.println();
}

void loop() {
  digitalWrite(ledBack, LOW);
  checkSensor(trigPinLeft, echoPinLeft, ledLeft);
  checkSensor(trigPinRight, echoPinRight, ledRight);
  checkSensor(trigPinFront, echoPinFront, ledFront);
  handleQwiic(ledBack);
  
  delay(500);
}
