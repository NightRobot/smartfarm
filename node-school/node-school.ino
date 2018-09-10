#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>


#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_AM2315.h>

#include "DHT.h"
#define DHTPIN 14     // what digital pin the DHT22 is conected to D5
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

HTTPClient http;
ESP8266WiFiMulti WiFiMulti;

BH1750 lightMeter(0x23);
Adafruit_AM2315 am2315;

//setting WiFi Connecting
//const char* ssid = "SmartfarmSansara9";
//const char* password = "kmutnbsansara";
//const char* ssid = "atz";
//const char* password = "11111111";
const char* ssid = "Sansara@Salawin9.0";
const char* password = "sansara9";
//const char* ssid = "hakmaitai";
//const char* password = "10508420243";

DHT dht(DHTPIN, DHTTYPE);
//setting static ip address
IPAddress local_ip = {192, 168, 0, 190};
IPAddress gateway = {192, 168, 0, 1};
IPAddress subnet = {255, 255, 255, 0};

unsigned long previousMillis = 0;
const long interval = 60000;
bool first = false;
bool failed = false;

const char* host = "http://www.hmtsmt.com";
String url = "";
String database = "smartfarm";

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
  //  Serial.println(F("BH1750 Test"));

  //    if (! am2315.begin()) {
  //      //    Serial.println("Sensor not found, check wiring & pullups!");
  //      while (1);
  //    }
  //  WiFi.config(local_ip, gateway, subnet);
  WiFiMulti.addAP(ssid, password);
  //  connectWifi(ssid, password );
}
String message = "";
void loop() {
  unsigned long currentMillis = millis();
  if ( currentMillis - previousMillis >= interval || !first || failed) {
    previousMillis = currentMillis;

    checking_WiFi();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)



    if ((WiFiMulti.run() == WL_CONNECTED)) {
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      //      float h = 80;
      //      // Read temperature as Celsius (the default)
      //      float t = 12;
      //sensor sentry 1
      if (isnan(t) || isnan(h)) {
        failed = true;
        Serial.println("Error sensor 3");
        delay(1000);
      } else {
        failed = false;

        //DHT22 - test
        Serial.print("DHT22 - test ");
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" *C ");




        message = "temp=";
        message += t;
        message += ",humid=";
        message += h;
        send_data_dht(host, database , message);
      }
      uint16_t lux = lightMeter.readLightLevel();
      //      uint16_t lux = 100;
      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");

      //send temp data form am2315
      //send air_humidity
      uint16_t humid = am2315.readHumidity();

      //      uint16_t humid = 50;
      Serial.print("Air humidity: ");
      Serial.print(humid);
      Serial.println(" %");
      //send air_temperature
      uint16_t temp = am2315.readTemperature();
      //      uint16_t temp = 12;

      Serial.print("Air temperature: ");
      Serial.print(temp);
      Serial.println(" %");
      if (isnan(temp) || isnan(humid) || isnan(lux)) {
        failed = true;
        Serial.println("Error");
        delay(1000);
      } else {
        failed = false;



        message = "temp=";
        message += temp;
        message += ",humid=";
        message += humid;
        message += ",brightness=";
        message += lux;

        send_data_am2315(host, database , message);

      }
    } else {
      Serial.println("WiFi not connected!");
      delay(1000);
    }
  }
  message = "";
}
void connectWifi(const char* ssid, const char* password) {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFiMulti.addAP(ssid, password);
  //  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println();
  delay(1000);
}
void checking_WiFi() {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("reconnect to ");
    Serial.println(ssid);
    WiFiMulti.addAP(ssid, password);
    while (WiFiMulti.run() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("WiFi currenly connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void send_data_dht(String host, String database, String datamassage) {

  digitalWrite(BUILTIN_LED, LOW);
  first = true;
  String  url = host + ":8086/write?db=";
  url += database;

  String message = "soil-sensor";
  message += ",sensor=";
  message += "1";
  message += ",region=";
  message += "maesareang";
  message += " " + datamassage;
  message += ",heap=";
  message += ESP.getFreeHeap();


  Serial.println(url);
  Serial.println(message);

  http.begin(url); //HTTP


  int httpCode = http.POST(message);
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      //Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  //led blink when send data
  digitalWrite(BUILTIN_LED, HIGH);
}
void send_data_am2315(String host, String database, String datamassage) {
  digitalWrite(BUILTIN_LED, LOW);
  first = true;
  String  url = host + ":8086/write?db=";
  url += database;

  String message = "main-sensor";
  //  message += ",type=";
  //  message += type;
  message += ",region=";
  message += "maesareang";
  message += " " + datamassage;
  //  message += " temp=";
  //  message += t_av;
  //  message += ",humid=";
  //  message += h_av;
  message += ",heap=";
  message += ESP.getFreeHeap();


  Serial.println(url);
  Serial.println(message);

  http.begin(url); //HTTP


  int httpCode = http.POST(message);
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      //Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  //led blink when send data
  digitalWrite(BUILTIN_LED, HIGH);

}

