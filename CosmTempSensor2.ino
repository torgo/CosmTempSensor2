/**
 * Cosm Arduino sensor GSM client example.
 *
 * This sketch demonstrates connecting an Arduino to Cosm (https://cosm.com),
 * using the new Arduino library to send and receive data and the new GSM library.
 *
 * Requirements
 *   * Arduino with (Telefonica-designed) GSM shield
 *   * Arduino software with version >= 1.0
 *   * An account at Cosm (https://cosm.com)
 *
 *   * A temperature sensor connected to pin 3
 *
 * Adapted 1 April 2013 by Dan Appelquist from a sample program
 * created 8th January, 2013 using code written by Adrian McEwen with
 * modifications by Sam Mulube
 *
 * Full tutorial available here: https://cosm.com/docs/quickstart/arduino.html
 *
 * This code is in the public domain.
 */

#include <Cosm.h>
#include <HttpClient.h>
#include <GSM.h>

#define API_KEY "" // your Cosm API key
#define FEED_ID xxxxx // your Cosm feed ID

// PIN Number
#define PINNUMBER "" // your sim PIN number if you have one (most don't)

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

  Serial.println("GSM Cosm Sensor Client Example");
  Serial.println("==============================");

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
// not used for this sample but useful if you want to get data back from cosm
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

    
