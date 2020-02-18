/* Testing MQ-2 GAS sensor with serial monitor
   Suitable for detecting of LPG, i-butane, propane, methane ,alcohol, Hydrogen or smoke
   More info: http://www.ardumotive.com/how-to-use-mq2-gas-sensor-en.html
   Dev: Michalis Vasilakis // Date: 11/6/2015 // www.ardumotive.com                     */

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

const int gasPin = A0; // GAS sensor output pin to Arduino analog A0 pin
const int buzzer = 14;
int gas = 0;

//SSID of your network
char ssid[] = "Qlue Biznet"; //SSID of your Wi-Fi router
char pass[] = "17agustus1945"; //Password of your Wi-Fi router

void setup()
{
  Serial.begin(9600); // Initialize serial port - 9600 bps
  pinMode(buzzer,OUTPUT);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop()
{
  gas = analogRead(gasPin);
  Serial.println(gas);
  if (gas > 300) {
    digitalWrite(buzzer, HIGH);
    sendRequest(gas);
  } else {
    digitalWrite(buzzer, LOW);
  }
  
  delay(500); // Print value every 0.5 sec.
}

bool sendRequest(int gasLevel) {
  // Reserve memory space for your JSON data
  /*
  * PAYLOAD MESSAGE
  */
  String postMessage;
  DynamicJsonDocument doc(2048);
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> JSONbuffer;
   
  doc["lat"] = -6.2682185;
  doc["lng"] = 106.8296376;
  doc["description"] = "warning methane levels is " + String(gasLevel) + " ppm";
  doc["address"] = "qlue pejaten barat";
  doc["place"] = "pejaten 13";

  // Serialize JSON document
   
  serializeJsonPretty(doc, Serial);


  Serial.print("POST ");

  serializeJsonPretty(doc,postMessage);

  sendToServer(postMessage);
  sendToServerSpeaker();

  Serial.println("Finished sending to AWS..");   
}

void sendToServer(String postMessage){
  Serial.println("Sending via HTTP for dashboard ticket");
  
  HTTPClient http;
  
  http.begin("http://ec2-13-250-132-156.ap-southeast-1.compute.amazonaws.com/v2/hackathon/report"); 
  http.addHeader("Content-Type", "application/json");
  
  int httpCode = http.POST(postMessage);
  
  if(httpCode > 0) {
    Serial.println("Ticket sent to Dashboard");
  } else {
    Serial.println("Failed send ticket to Dashboard");
  }
}

void sendToServerSpeaker(){
  Serial.println("Sending via HTTP for speaker");
  
  HTTPClient http;
  
  http.begin("http://192.168.10.169:3000/tts?person=123,Bahaya%20level%20gas%20metan%20tinggi%20mohon%20agar%20segera%20evakuasi");
  http.addHeader("Content-Type", "application/json");
  
  int httpCode = http.GET();
  
  if(httpCode > 0) {
    Serial.println("Smart Speaker OK!");
  } else {
    Serial.println("Smart Speaker NOT OK!");
  }
}
