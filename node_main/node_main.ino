#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_AM2315.h>

//define valve control
#define VALVE1 D5
#define VALVE2 D6
#define VALVE3 D7
#define VALVE4 D8

HTTPClient http;
ESP8266WiFiMulti WiFiMulti;

BH1750 lightMeter(0x23);
Adafruit_AM2315 am2315;

//setting WiFi Connecting

const char* ssid = "SmartfarmSansara9";
const char* password = "kmutnbsansara";

//const char* ssid = "atz";
//const char* password = "11111111";

//const char* ssid = "Sansara@Salawin9.0";
//const char* password = "sansara9";

//const char* ssid = "hakmaitai";
//const char* password = "10508420243";


unsigned long previousMillis_s = 0;
unsigned long previousMillis_v = 0;
const long interval_sensor = 60000;
const long interval_valve = 20000;
bool first = false;
bool failed = false;

//setting host connection
String host = "http://www.sansara9smartfarm.com";
String url = "";
String database = "smartfarm";

String status_text = "";
//setting static ip address
IPAddress local_ip = {192, 168, 1, 180};
IPAddress gateway = {192, 168, 1, 1};
IPAddress subnet = {255, 255, 255, 0};

void setup()
{

  pinMode(VALVE1, OUTPUT);     // Initialize the VALVE1 pin as an output
  pinMode(VALVE2, OUTPUT);     // Initialize the VALVE2 pin as an output
  //not use valve 3 and 4
  //  pinMode(VALVE3, OUTPUT);     // Initialize the VALVE3 pin as an output
  //  pinMode(VALVE4, OUTPUT);     // Initialize the VALVE4 pin as an output
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(VALVE1, HIGH);
  digitalWrite(VALVE2, HIGH);
  Serial.begin(115200);

  Wire.begin();
  lightMeter.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
  //  Serial.println(F("BH1750 Test"));

  //    if (! am2315.begin()) {
  //      //    Serial.println("Sensor not found, check wiring & pullups!");
  //      while (1);
  //    }

  WiFi.config(local_ip, gateway, subnet);
  //  connectWifi(ssid, password);
  WiFiMulti.addAP(ssid, password);

}
void loop() {
  unsigned long currentMillis = millis();
  if ( currentMillis - previousMillis_s >= interval_sensor || !first || failed) {
    previousMillis_s = currentMillis;

    checking_WiFi();
    //  send brightness
    uint16_t lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    //  send temp data form am2315
    //send air_humidity
    uint16_t humid = am2315.readHumidity();
    Serial.print("Air humidity: ");
    Serial.print(humid);
    Serial.println(" %");
    //send air_temperature
    uint16_t temp = am2315.readTemperature();
    Serial.print("Air temperature: ");
    Serial.print(temp);
    Serial.println(" %");


    if ((WiFiMulti.run() == WL_CONNECTED)) {
      if (isnan(temp) || isnan(humid) || isnan(lux)) {
        failed = true;
        Serial.println("Error");
        delay(1000);
      } else {
        failed = false;
        String datamassage = "temp=";
        datamassage += temp;
        datamassage += ",humid=";
        datamassage += humid;
        datamassage += ",brightness=";
        datamassage += lux;
        send_data(host, database , datamassage);

      }
    }
  }
  if ( currentMillis - previousMillis_v >= interval_valve || !first || failed) {
    previousMillis_v = currentMillis;
    //send get req for check valve 2
    String api = "/api/get/valve_status/1";
    checking_status_valve1();

    //    send get req for check valve 2
    api = "/api/get/valve_status/2";
    checking_status_valve2();
  }




}
//void connectWifi(const char* ssid, const char* password) {
//  Serial.println();
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//
//  //  WiFi.begin(ssid, password);
//
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//
//  Serial.println("");
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
//  Serial.println();
//  Serial.println();
//  delay(1000);
//}
void checking_WiFi() {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("reconnect to ");
    Serial.println(ssid);
    //    WiFi.begin(ssid, password);
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


void send_data(String host, String database, String datamassage) {

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

void checking_status_valve1() {

  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server


  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
  http.begin("http://www.sansara9smartfarm.com/api/get/valve_status/1"); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      //      int find_o = payload.indexOf('O');
      int find_f = payload.indexOf('F');
      int find_n = payload.indexOf('N');
      if (find_f == 0 && find_n == 1) {
        Serial.println("ON state");
        digitalWrite(VALVE1, LOW);

        //      break;
      } else if (find_f == 1 && find_n == 0) {
        Serial.println("OFF state");
        digitalWrite(VALVE1, HIGH);

        //      break;
      }
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

}
void checking_status_valve2() {


  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server


  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
  http.begin("http://www.sansara9smartfarm.com/api/get/valve_status/2"); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
      //      int find_o = payload.indexOf('O');
      int find_f = payload.indexOf('F');
      int find_n = payload.indexOf('N');
      if (find_f == 0 && find_n == 1) {
        Serial.println("ON state");
        digitalWrite(VALVE2, LOW);

        //      break;
      } else if (find_f == 1 && find_n == 0) {
        Serial.println("OFF state");
        digitalWrite(VALVE2, HIGH);

        //      break;
      }
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

