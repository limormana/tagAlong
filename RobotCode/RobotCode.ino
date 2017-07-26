#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <SparkFun_RFD77402_Arduino_Library.h> //Use Library Manager or download here: https://github.com/sparkfun/SparkFun_RFD77402_Arduino_Library
RFD77402 myDistance; //Hook object to the library

#define REVERSE_SPEED     100 // 0 is stopped, 400 is full speed
#define TURN_SPEED        150
#define FORWARD_SPEED     250
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms
#define RUNNING_DURATION  10000 // ms

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum
const int distanceTH = 200;  //TH value for ditacting an obstcale
unsigned long startTime = millis();
int stage = 1; // 1- Obstcale avoidance 2- Wait for chair

/// Led /////
const int ledPin =  4;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = millis();        // will store last time LED was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)

void setup()
{
    // Set the digital pin as output.
    pinMode(ledPin, OUTPUT);
  
    button.waitForButton();
    // initialize serial communication @ 9600 baud:
    Serial.begin(115200);

    //while (!Serial) ;
    //Serial.println("RFD77402 Read Example");

    if (myDistance.begin() == false)
    {
      //  Serial.println("Sensor failed to initialize. Check wiring.");
        while (1) ; //Freeze!
    }
    //Serial.println("Sensor online!");
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}

void loop()
{
    // Obstcale Avoidance Mode
    if (stage == 1)
    {
        // Led blinks
        // check to see if it's time to blink the LED; that is, if the difference
        // between the current time and last time you blinked the LED is bigger than
        // the interval at which you want to blink the LED.
        unsigned long currentMillis = millis();
      
        if (currentMillis - previousMillis >= interval) 
        {
            // save the last time you blinked the LED
            previousMillis = millis();
        
            // if the LED is off turn it on and vice-versa:
            if (ledState == LOW) {
              ledState = HIGH;
            } else {
              ledState = LOW;
            }
        
            // set the LED with the ledState of the variable:
            digitalWrite(ledPin, ledState);
          }
        ///// Led Logic END///////////
               
        ///// Obstacles Logic ///////
        byte errorCode = myDistance.takeMeasurement();
        if (errorCode == CODE_VALID_DATA)
        {
            unsigned int distance = myDistance.getDistance();
            byte pixels = myDistance.getValidPixels();
            unsigned int confidence = myDistance.getConfidenceValue();
            int rand;

        /*    Serial.print("distance: ");
            Serial.print(distance);
            Serial.print("mm pixels: ");
            Serial.print(pixels);
            Serial.print(" confidence: ");
            Serial.print(confidence);
*/
            // Identified an obstcale
            if (distance < distanceTH)
            {
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
  /*      else if (errorCode == CODE_FAILED_PIXELS)
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
*/
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
        // Constant Led Light
        digitalWrite(ledPin, HIGH);
        
        while(Serial.available()) {
          char t = Serial.read();
        }
        delay(10);
        
        if(Serial.available())
        {
          String message = Serial.readStringUntil('\r\n');
          //Serial.println(message);
          if (message.indexOf('-') != message.lastIndexOf('-'))
            message = message.substring(0, message.lastIndexOf('-'));
          /*
          char message[20] = {0};
          int i =0, j = 0;
          while(Serial.available())
          {
              message[i++] = Serial.read();  
          } 
          Serial.flush();*/

          int distance = atoi(message.c_str());
          Serial.println("Message: " + message);
          //Serial.print("Distance: ");
          //erial.println(distance);
          if(strlen(message.c_str()) > 0 && distance > -40)
          {
            motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
            stage = 1;
            startTime = millis();
          }
        }
    }

    
}
