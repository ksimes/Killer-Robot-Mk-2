/*
   Sensorium UltraSonic sensor

  HC-SR04 Ping distance sensor:
  VCC to arduino 5v
  GND to arduino GND
  Echo to Arduino pin 7
  Trig to Arduino pin 8

  This sketch originates from Virtualmix: http://goo.gl/kJ8Gl
  Has been modified by Winkle ink here: http://winkleink.blogspot.com.au/2012/05/arduino-hc-sr04-ultrasonic-distance.html
  And modified further by ScottC here: http://arduinobasics.blogspot.com.au/2012/11/arduinobasics-hc-sr04-ultrasonic-sensor.html
  on 10 Nov 2012.
  Converted to GKR Sensorium on 8th Feb 2020 by S. King
*/
#include <ArduinoJson.h>
#include "messages.h"

#define VERSION "1.0"
// Serial comms speed
#define SERIAL_SPEED 115200

const int pingPin = 7;
const int echoPin = 4;

const String station = "Black";
// Time to sleep between taking samples (in milliseconds)
const long sleepTimeS = 2000;
const String sensorType = "ULTRA";   // What is the SensorType

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

// Message processor comming in from Rasp Pi overController
Messages *messages;
// Allocate the JSON documents
//
// Inside the brackets, <> is the RAM allocated to this document.
// Change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.

StaticJsonDocument<150> postData;
JsonObject distanceJSON;

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

  Serial.print("{\"Title\": \"UltraSonic Sensor version ");
  Serial.print(VERSION);
  Serial.println("\" }");

  messages = new Messages();

  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Add fixed data to the document
  //
  postData["stationId"] = station;
  postData["sampleRate"] = sleepTimeS;
  postData["sensorType"] = sensorType;
  // For memory usage you need to create nested structures once in setup and then just populate the attributes as required
  // when you read the data.
  distanceJSON = postData.createNestedObject("distance");
}

void loop() {
  // put your main code here, to run repeatedly:

  // Anything from the overController?
  if (messages->msgAvalable()) {
    String msg = messages->read(false);
  }

  // Anything from the sensorium? i.e. read by the sensor
  {
    long duration, inches, cm;

    /* The following trigPin/echoPin cycle is used to determine the
      distance of the nearest object by bouncing soundwaves off of it. */
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);

    duration = pulseIn(echoPin, HIGH);

    // convert the time into a distance
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);

    //    if (cm >= maximumRange || cm <= minimumRange){
      //Results
      distanceJSON["cm"] = cm;
      distanceJSON["inches"] = inches;
      sendData();
    //    }
  }

  yield();
  delay(sleepTimeS);
}


long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
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
