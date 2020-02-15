/*
   Sensorium Temperature sensor

  Make sure the sensor is found, you can also pass in a different i2c
  address with tempsensor.begin(0x19) for example, also can be left in blank for default address use
  Also there is a table with all addres possible for this sensor, you can connect multiple sensors
  to the same i2c bus, just configure each sensor with a different address and define multiple objects for that
  A2 A1 A0 address
  0  0  0   0x18  this is the default address
  0  0  1   0x19
  0  1  0   0x1A
  0  1  1   0x1B
  1  0  0   0x1C
  1  0  1   0x1D
  1  1  0   0x1E
  1  1  1   0x1F

  Up to 8 temp sensors can be supported of on the one I2C bus.

  Converted to GKR Sensorium on 8th Feb 2020 by S. King
*/
#include "messages.h"
#include "Adafruit_MCP9808.h"
#include <ArduinoJson.h>

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

#define VERSION "1.0"
// Serial comms speed
#define SERIAL_SPEED 115200

const String station = "yellow";
// Time to sleep between taking samples (in milliseconds)
const long sleepTimeS = 5000;
const String sensorType = "TEMP";   // What is the SensorType

// Message processor comming in from Rasp Pi controller
Messages *messages;
// Allocate the JSON documents
//
// Inside the brackets, <> is the RAM allocated to this document.
// Change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.

StaticJsonDocument<150> postData;
JsonObject temperature;

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
  
  Serial.print("{\"Title\": \"Temperature Sensor version ");
  Serial.print(VERSION);
  Serial.println("\" }");

  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808 Sersor! Check your connections.");
    while (1);
  }

  tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  messages = new Messages();

  // Add fixed data to the document
  //
  postData["stationId"] = station;
  postData["sampleRate"] = sleepTimeS;
  postData["sensorType"] = sensorType;
  // For memory usage you need to create nested structures once in setup and then just populate the attributes as required
  // when you read the data.
  temperature = postData.createNestedObject("temperature");
}

void loop() {
  // put your main code here, to run repeatedly:

  // Anything from the controller?
  if (messages->msgAvalable()) {
    String msg = messages->read(false);
  }

  // Anything from the sensorium? i.e. read by the sensor
  {
    tempsensor.wake();   // wake up, ready to read!
    int res = tempsensor.getResolution();
    float c = tempsensor.readTempC();

    //Results
    temperature["Resolution"] = res;
    temperature["Celsius"] = c;

    sendData();
    tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
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
