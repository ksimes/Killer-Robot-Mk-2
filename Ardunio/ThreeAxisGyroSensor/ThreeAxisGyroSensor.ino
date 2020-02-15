/*
   Sensorium Three Axis Gyro sensor

   MMA8451
   This is an example for the Adafruit MMA8451 Accel breakout board
   ----> https://www.adafruit.com/products/2019

  Converted to GKR Sensorium on 9th Feb 2020 by S. King
*/
#include "messages.h"
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>      // version 6.14.1

#define VERSION "1.0"
// Serial comms speed
#define SERIAL_SPEED 115200

Adafruit_MMA8451 mma = Adafruit_MMA8451();

const String station = "Red";
// Time to sleep between taking samples (in milliseconds)
const long sleepTimeS = 10000;
const String sensorType = "GYR";   // What is the SensorType

// Message processor comming in from Rasp Pi controller
Messages *messages;
// Allocate the JSON documents
//
// Inside the brackets, <> is the RAM allocated to this document.
// Change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.

StaticJsonDocument<300> postData;
JsonObject positionJSON, rawJSON, accelerationJSON, orientationJSON;

// Returns data as a JSON message to the controller
static void sendData()
{
  // Generate the minified JSON and send it to the Serial port.
  serializeJson(postData, Serial);

  // So we get the 'cr' seperator for the recieving program to seperate the messages
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(SERIAL_SPEED);
  while (!Serial); //waits for serial terminal to be open, necessary in newer arduino boards.

  Serial.print("{\"Title\": \"ThreeAxisGyroSensor version ");
  Serial.print(VERSION);
  Serial.println("\" }");

  messages = new Messages();

  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }

  // Add fixed data to the document
  //
  postData["stationId"] = station;
  postData["sampleRate"] = sleepTimeS;
  postData["sensorType"] = sensorType;
  // For memory usage you need to create nested structures once in setup and then just populate the attributes as required
  // when you read the data.
  positionJSON = postData.createNestedObject("position");
  rawJSON = positionJSON.createNestedObject("raw");
  accelerationJSON = positionJSON.createNestedObject("acceleration");
}

void loop() {
  // put your main code here, to run repeatedly:

  // Anything from the controller?
  if (messages->msgAvalable()) {
    String msg = messages->read(false);
  }

  // Anything from the sensorium? i.e. read by the sensor
  {
    // Read the 'raw' data in 14-bit counts
    mma.read();

    //Results
    rawJSON["X"] = mma.x;
    rawJSON["Y"] = mma.y;
    rawJSON["Z"] = mma.z;

    /* Get a new sensor event */
    sensors_event_t event;
    mma.getEvent(&event);

    /* Display the results (acceleration is measured in m/s^2) */
    accelerationJSON["X"] = event.acceleration.x;
    accelerationJSON["Y"] = event.acceleration.y;
    accelerationJSON["Z"] = event.acceleration.z;
    accelerationJSON["measure"] = "m/s^2";

    /* Get the orientation of the sensor */
    uint8_t o = mma.getOrientation();

    switch (o) {
      case MMA8451_PL_PUF:
        positionJSON["orientation"] = "Portrait Up Front";
        break;
      case MMA8451_PL_PUB:
        positionJSON["orientation"] = "Portrait Up Back";
        break;
      case MMA8451_PL_PDF:
        positionJSON["orientation"] = "Portrait Down Front";
        break;
      case MMA8451_PL_PDB:
        positionJSON["orientation"] = "Portrait Down Back";
        break;
      case MMA8451_PL_LRF:
        positionJSON["orientation"] = "Landscape Right Front";
        break;
      case MMA8451_PL_LRB:
        positionJSON["orientation"] = "Landscape Right Back";
        break;
      case MMA8451_PL_LLF:
        positionJSON["orientation"] = "Landscape Left Front";
        break;
      case MMA8451_PL_LLB:
        positionJSON["orientation"] = "Landscape Left Back";
        break;
      default:
        positionJSON["orientation"] = "Unknown";
        break;
    }

    sendData();
  }

  yield();
  delay(sleepTimeS);
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  messages->anySerialEvent();
}
