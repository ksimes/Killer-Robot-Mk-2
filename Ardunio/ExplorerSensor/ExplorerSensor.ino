/*
   Sensorium template processor
*/
#include <ArduinoJson.h>
#include "messages.h"

#define VERSION "1.0"
// Serial comms speed
#define SERIAL_SPEED 115200

const String station = "station";
// Time to sleep between taking samples (in seconds)
const long sleepTimeS = 1000;

const String sensorType = "EXAMPLE";   // What is the SensorType

// Message processor comming in from Rasp Pi overController
Messages *messages;
// Allocate the JSON documents
//
// Inside the brackets, <> is the RAM allocated to this document.
// Change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.

StaticJsonDocument<100> postData;
JsonObject location;

// Returns data as a JSON message to the overController
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

  Serial.print("Explorer Controller version ");
  Serial.println(VERSION);

  messages = new Messages();

  // Add fixed data to the document
  //
  postData["stationId"] = station;
  postData["sampleRate"] = sleepTimeS;
  postData["sensorType"] = sensorType;
  // For memory usage you need to create nested structures once in setup and then just populate the attributes as required
  // when you read the data.
  location = postData.createNestedObject("location");
}

void loop() {
  // put your main code here, to run repeatedly:

  // Anything from the overController?
  if (messages->msgAvalable()) {
    String msg = messages->read(false);
  }

  // Anything from the sensorium? i.e. read by the chip
  {
    // Add data captured from sensor here
    // Add a "location" object
    location["sensor"] = "gps";
    location["lat"] = 48.748010;
    location["lon"] = 2.293491;    
    // above is example data. Remove.

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
