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

unsigned long startTime = millis();

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, HIGH);

  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  int rand;
  
  if(millis() - startTime > 2000)   // go forward for 2 sec
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

    startTime = millis();
  }
}
