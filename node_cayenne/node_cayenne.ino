// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>
#include <Wire.h>
#include <Adafruit_AM2315.h>


// WiFi network info.
char ssid[] = "xp_atz";
char wifiPassword[] = "11111111";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "8e1dc790-c05c-11e7-993a-9b80361bb997";
char password[] = "fd1fa87442df7176f986973ca121e9a3a25a98d7";
char clientID[] = "fafe8d90-e127-11e7-8123-07faebe02555";

unsigned long lastMillis = 0;

Adafruit_AM2315 am2315;

void setup() {
  Serial.begin(9600);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);

  if (! am2315.begin()) {
     Serial.println("Sensor not found, check wiring & pullups!");
     while (1);
  }
}

void loop() {
  Cayenne.loop();
  Serial.print("Hum: "); Serial.println(am2315.readHumidity());
  Serial.print("Temp: "); Serial.println(am2315.readTemperature());

  delay(1000);
  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
    Cayenne.virtualWrite(0, lastMillis);
    Cayenne.celsiusWrite(1, am2315.readTemperature());
    Cayenne.virtualWrite(2,am2315.readHumidity());
  }
}

//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
