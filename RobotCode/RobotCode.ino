/*
  Getting distance from the RFD77402 Time of Flight Sensor
  By: Nathan Seidle
  SparkFun Electronics
  Date: June 6th, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!

  Read the raw distance values from the vertical-cavity surface-emitting laser (VCSEL) sensor.

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield (any port)
  Open the serial monitor at 9600 baud

  Available:
  takeMeasurement() - Initiates measurement. Read via getDistance(), etc

  getDistance() - Returns the last measurement value
  getValidPixels() - Returns number of valid pixels
  getConfidenceValue() - A qualitative value of the distance measurement

  getPeak/setPeak(byte) - Gets/sets the vertical-cavity surface-emitting laser (VCSEL) peak
  getThreshold/setThreshold(byte) - Gets/sets the VCSEL threshold
  getFrequency/setFrequency(byte) - Gets/sets the modulation frequency

  goToOffMode() - Turn off MCPU
  goToOnMode() - Wake MCPU to ON Mode
  goToStandbyMode() - Low power standby mode
  getMeasurement() - Once sensor is configured, issue this command to take measurement

  getCalibrationData() - reads 27 messages of MPU mailbox data and loads calibration data array
  getMailbox() - returns the 16-bits in the MPU mailbox

  reset() - Resets IC to initial POR settings

*/
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <SparkFun_RFD77402_Arduino_Library.h> //Use Library Manager or download here: https://github.com/sparkfun/SparkFun_RFD77402_Arduino_Library
RFD77402 myDistance; //Hook object to the library

#define REVERSE_SPEED     100 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     300
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms
#define RUNNING_DURATION  10000 // ms

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
unsigned long startTime = millis();
int stage = 1; // 1- Obstcale avoidance 2- Wait for chair
void setup()
{
    button.waitForButton();
    // initialize serial communication @ 9600 baud:
    Serial.begin(9600);

    while (!Serial) ;
    Serial.println("RFD77402 Read Example");

    if (myDistance.begin() == false)
    {
        Serial.println("Sensor failed to initialize. Check wiring.");
        while (1) ; //Freeze!
    }
    Serial.println("Sensor online!");
}

void loop()
{
    // Obstcale Avoidance Mode
    if (stage == 1)
    {
        ///// Obstacles Logic ///////
        byte errorCode = myDistance.takeMeasurement();
        if (errorCode == CODE_VALID_DATA)
        {
            unsigned int distance = myDistance.getDistance();
            byte pixels = myDistance.getValidPixels();
            unsigned int confidence = myDistance.getConfidenceValue();
            int rand;

            Serial.print("distance: ");
            Serial.print(distance);
            Serial.print("mm pixels: ");
            Serial.print(pixels);
            Serial.print(" confidence: ");
            Serial.print(confidence);

            // Identified an obstcale
            if (distance < 200)
            {
                // put your main code here, to run repeatedly:  
                rand = random(1, 3);
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

        ///// Obstacles Logic END///////

        if (millis() - startTime > RUNNING_DURATION)
        {
            // Keep turning until identifing the chair
            motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
            stage = 2;
        }
    }

    // Wait for wheelchair Mode
    else
    {
        int sensorReading = analogRead(A1);
        // map the sensor range (four options):
        // ex: 'long int map(long int, long int, long int, long int, long int)'
        int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
        // if infra, turn around itself
        if (range == 1 || range == 0)   // go forward for 2 sec
        {
            motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
            stage = 1;
            startTime = millis();
        }
    }
}
