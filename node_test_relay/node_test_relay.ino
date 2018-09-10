#include <ESP8266WiFi.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_AM2315.h>
BH1750 lightMeter(0x23);
Adafruit_AM2315 am2315;

void setup()
{
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();


  Serial.begin(115200);   //ตั้งค่าใช้งาน serial ที่ baudrate 115200
  delay(10);
  Serial.println();
  Serial.println();
  connectWifi("Smartfarm_Sansara@9", "kmutnbsansara");
  pinMode(D5, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

//  lightMeter.begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
//  Serial.println(F("BH1750 Test"));
//
//  if (! am2315.begin()) {
//    Serial.println("Sensor not found, check wiring & pullups!");
//    while (1);
//  }
}
void loop() {
//  uint16_t lux = lightMeter.readLightLevel();
//
//  Serial.print("Light: ");
//  Serial.print(lux);
//  Serial.println(" lx");
  //  String data = lux;
  //  send_data("35.197.132.76","/insert/brightness/",lux);
  //  delay(5000);


//  Serial.print("Hum: ");
//  uint16_t humid = am2315.readHumidity();
//  Serial.println(humid);
  //  send_data("35.197.132.76","/insert/humidity/",humid);
  //  delay(5000);


//  Serial.print("Temp: ");
//  uint16_t temp = am2315.readTemperature();
//  Serial.println(temp);
  //  send_data_all("35.197.132.76","/insert/",temwp,humid,lux);
//  delay(10000);
  
  
  //http://35.198.211.170/api/get/valve_status/1/1
  checking_status("35.198.211.170","/api/get/valve_status/1/1");
  
  //send_data("35.198.211.170", "/insert/temperature/", temp);
    delay(5000);
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
  Serial.println("");
  Serial.println("WiFi connected");   //แสดงข้อความเชื่อมต่อสำเร็จ
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //แสดงหมายเลข IP ของ ESP8266(DHCP)
}

void send_data(const char* host, String url, uint16_t data) {
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
  url += data;

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
    if (line.startsWith("HTTP/1.1 200 OK")) {
      Serial.println("send successfull!");

      break;
    }
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection");
}
void checking_status(const char* host, String url) {
  Serial.print("connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

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
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line.startsWith("ON")) {
      Serial.println("ON state");
      digitalWrite(D5, LOW);
//      break;
    } else if(line.startsWith("OFF")){
      Serial.println("OFF state");
      digitalWrite(D5, HIGH);
//      break;
    }
  }
//  while(client.available()){
//    String line = client.readStringUntil('\r');
//    Serial.print(line);
//  }
  Serial.println();
  Serial.println("closing connection");
}
void relay(String status){
  if(status == "ON"){
    Serial.println("turn on");
    digitalWrite(LED_BUILTIN, LOW);
    delay(10000);
    }else if (status == "OFF"){
      Serial.println("turn off");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10000);
    }
  }
