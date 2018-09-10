#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "DHT.h"
#define DHTPIN1 14     // what digital pin the DHT22 is conected to D5
#define DHTPIN2 12     // what digital pin the DHT22 is conected to D6
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

HTTPClient http;
ESP8266WiFiMulti WiFiMulti;

//setting WiFi Connecting
const char* ssid = "SmartfarmSansara9";
const char* password = "kmutnbsansara";
//const char* ssid = "atz";
//const char* password = "11111111";


DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

//setting static ip address
//sensor 1,2 ip 192.168.1.141
//sensor 3,4 ip 192.168.1.142
IPAddress local_ip = {192, 168, 1, 142};
IPAddress gateway = {192, 168, 1, 1};
IPAddress subnet = {255, 255, 255, 0};

unsigned long previousMillis = 0;
const long interval = 60000;
bool first = false;
bool failed = false;

const char* host = "http://www.sansara9smartfarm.com";
String url = "";
String database = "smartfarm";

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  WiFi.config(local_ip, gateway, subnet);
  connectWifi(ssid, password );
}

void loop() {
  unsigned long currentMillis = millis();
  if ( currentMillis - previousMillis >= interval || !first || failed) {
    previousMillis = currentMillis;

    checking_WiFi();
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h1 = dht1.readHumidity();
    float h2 = dht2.readHumidity();
    // Read temperature as Celsius (the default)
    float t1 = dht1.readTemperature();
    float t2 = dht2.readTemperature();

    if ((WiFiMulti.run() == WL_CONNECTED)) {
      //sensor sentry 1
      if (isnan(t1) || isnan(h1)) {
        failed = true;
        Serial.println("Error sensor 3");
        delay(1000);
      } else {
        failed = false;

        //DHT22 - test
        Serial.print("DHT22 - test ");
        Serial.print("Humidity: ");
        Serial.print(h1);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t1);
        Serial.println(" *C ");
        String datamassage = "temp=";
        datamassage += t1;
        datamassage += ",humid=";
        datamassage += h1;
        send_data(host, database , "3", datamassage);
      }
      //sensor sentry 2
      if (isnan(t2) || isnan(h2)) {
        failed = true;
        Serial.println("Error sensor 4");
        delay(1000);
      } else {
        failed = false;

        //DHT22 - test
        Serial.print("DHT22 - test ");
        Serial.print("Humidity: ");
        Serial.print(h2);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t2);
        Serial.println(" *C ");
        String datamassage = "temp=";
        datamassage += t2;
        datamassage += ",humid=";
        datamassage += h2;
        send_data(host, database , "4", datamassage);
      }
    } else {
      Serial.println("WiFi not connected!");
      delay(1000);
    }
  }
}
void connectWifi(const char* ssid, const char* password) {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("WiFi currenly connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void send_data(String host, String database, String sensor, String datamassage) {

  digitalWrite(BUILTIN_LED, LOW);
  first = true;
  String  url = host + ":8086/write?db=";
  url += database;

  String message = "soil-sensor";
  message += ",sensor=";
  message += sensor;
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


