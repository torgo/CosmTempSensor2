/**
 * Cosm Arduino sensor client example.
 *
 * This sketch demonstrates connecting an Arduino to Cosm (https://cosm.com),
 * using the new Arduino library to send and receive data.
 *
 * Requirements
 *   * Arduino with Ethernet shield or Arduino Ethernet (board must use the
 *     Wiznet Ethernet chipset)
 *   * Arduino software with version >= 1.0
 *   * An account at Cosm (https://cosm.com)
 *
 * Optional
 *   * An analog sensor connected to pin 2 (note we can still read a value from
 *     the pin without this)
 *
 * Created 8th January, 2013 using code written by Adrian McEwen with
 * modifications by Sam Mulube
 *
 * Full tutorial available here: https://cosm.com/docs/quickstart/arduino.html
 *
 * This code is in the public domain.
 */

#include <Cosm.h>
#include <HttpClient.h>
// #include <SPI.h>
#include <GSM.h>

#define API_KEY "8bePI6m3m3kjklpXw0MYxu9vtsuSAKxiL3dwUTVHb2QyTT0g" // your Cosm API key
#define FEED_ID 121725 // your Cosm feed ID

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "bluevia.movistar.es"  // replace your GPRS APN
#define GPRS_LOGIN     ""     // replace with your GPRS login
#define GPRS_PASSWORD  ""  // replace with your GPRS password

// initialize the library instance:
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int sensorPin = 3;

unsigned long lastConnectionTime = 0;                // last time we connected to Cosm
const unsigned long connectionInterval = 900000;      // delay between connecting to Cosm in milliseconds

// Initialize the Cosm library

// Define the string for our datastream ID
char sensorId[] = "sensor_reading";

CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
};

// Wrap the datastream into a feed
CosmFeed feed(FEED_ID, datastreams, 1 /* number of datastreams */);

CosmClient cosmclient(client);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Cosm Sensor Client Example");
  Serial.println("==========================");

}

void loop() {
  // main program loop
  if (millis() - lastConnectionTime > connectionInterval) {
    // read a value from the pin
    int sensorValue = analogRead(sensorPin);
    float voltage = sensorValue * (5.0 / 1023.0);
    float temperature = (voltage - .5) * 100;
    // initialize GPRS connection
    Serial.println("Initializing network");

    // connection state
    boolean notConnected = true;
  
    // After starting the modem with GSM.begin()
    // attach the shield to the GPRS network with the APN, login and password
    while(notConnected)
    {
      if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
          (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
        notConnected = false;
      else
      {
        Serial.println("Not connected");
        delay(1000);
      }
    }
    Serial.println("Network initialized");
    Serial.println();

    // send it to Cosm
    sendData(temperature);
    // read the datastream back from Cosm
    // getData();
    // disconnect the GPRS connection
    Serial.println("Network disconnected");  
    client.stop();  
    // update connection time so we wait before connecting again
    lastConnectionTime = millis();
  }
}

// send the supplied value to Cosm, printing some debug information as we go
void sendData(float sensorValue) {
  datastreams[0].setFloat(sensorValue);

  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());

  Serial.println("Uploading to Cosm");
  int ret = cosmclient.put(feed, API_KEY);
  Serial.print("PUT return code: ");
  Serial.println(ret);

  Serial.println();
}

// get the value of the datastream from Cosm, printing out the value we received
void getData() {
  Serial.println("Reading data from Cosm");

  float ret = cosmclient.get(feed, API_KEY);
  Serial.print("GET return code: ");
  Serial.println(ret);

  if (ret > 0) {
    Serial.print("Datastream is: ");
    Serial.println(feed[0]);

    Serial.print("Sensor value is: ");
    Serial.println(feed[0].getFloat());
  }

  Serial.println();
}

    
