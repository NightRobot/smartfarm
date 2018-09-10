
//#include <Wire.h>
//#include <Adafruit_AM2315.h>

//Adafruit_AM2315 am2315;
#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN 14     // what digital pin the DHT22 is conected to D0
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

//setting WiFi Connecting
//const char* ssid = "Smartfarm_Sansara@9";
//const char* password = "kmutnbsansara";

const char* ssid = "xp_atz";
const char* password = "11111111";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  //  connectWifi("xp_atz","11111111");

  connectWifi(ssid,password );
  //  if (! am2315.begin()) {
  //    Serial.println("Sensor not found, check wiring & pullups!");
  //    while (1);
  //  }

}

int timeSinceLastRead = 0;
void loop() {
  checking_WiFi();
  // Report every 2 seconds.
  // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    //DHT22 - test
    Serial.print("DHT22 - test ");
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");
    //send moisture
    String url = "/api/insert/soil_moisture/1/1/";
    url += h;

    send_data("35.198.211.170", url);
    //send temp
    url = "/api/insert/soil_temperature/1/1/";
    url += t;

    send_data("35.198.211.170", url);
    //am2315 - test
    //    Serial.print("AM2315 - test ");
    //    Serial.print("Humidity: ");
    //    Serial.print(am2315.readHumidity());
    //    Serial.print(" %\t");
    //    Serial.print("Temperature: ");
    //    Serial.print(am2315.readTemperature());
    //    Serial.println(" *C ");
  delay(10000);
}
void connectWifi(const char* ssid, const char* password) {
  Serial.print("Connecting to "); //แสดงข้อความ  “Connecting to”
  Serial.println(ssid);       //แสดงข้อความ ชื่อ SSID
  WiFi.begin(ssid, password); // เชื่อมต่อไปยัง AP

  while (WiFi.status() != WL_CONNECTED)   //รอจนกว่าจะเชื่อมต่อสำเร็จ
  {

    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connect");   //แสดงข้อความเชื่อมต่อสำเร็จ
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //แสดงหมายเลข IP ของ ESP8266(DHCP)
}
void checking_WiFi() {
  if (WiFi.status() != WL_CONNECTED)   //รอจนกว่าจะเชื่อมต่อสำเร็จ
  {
    Serial.print("reconnect to "); //แสดงข้อความ  “Connecting to”
    Serial.println(ssid);       //แสดงข้อความ ชื่อ SSID
    WiFi.begin(ssid, password); // เชื่อมต่อไปยัง AP
    while (WiFi.status() != WL_CONNECTED)   //รอจนกว่าจะเชื่อมต่อสำเร็จ
    {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("WiFi currenly connected");   //แสดงข้อความเชื่อมต่อสำเร็จ
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //แสดงหมายเลข IP ของ ESP8266(DHCP)
}
void send_data(const char* host, String url) {
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // We now create a URI for the request
  //  String url = "/insert/brightness/";
  //    url += data;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    //    Serial.println(line);
    if (line.startsWith("HTTP/1.1 200 OK")) {
      Serial.println("send successfull!");

      break;
    }
  }
  Serial.println();
  Serial.println("closing connection");
}

