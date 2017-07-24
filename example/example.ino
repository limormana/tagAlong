#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

#define LED 13

#define REVERSE_SPEED     100 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     100
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, HIGH);

  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);

   // initialize serial communication @ 9600 baud:
  Serial.begin(9600);
 }

void loop() {
  // read the sensor on analog A0:
  int sensorReading = analogRead(A1);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  int rand;
  
  // put your main code here, to run repeatedly:  
  if(range == 1 || range == 0)   // go forward for 2 sec
  {
    rand = random(1,3);
    if (rand == 1) //turn right
    {
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
    else
    {
       motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
       delay(TURN_DURATION);
       motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    }
  }
}
