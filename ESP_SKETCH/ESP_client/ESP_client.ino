#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

String relayName = "lights";
String relayStatus = "0";
String relayIP = "192.168.0.101";

short relayPin = 14;
unsigned long timer;

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 101);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

//SSID and Password to connect to wifi
const char *ssid = "Tobik_Hata";
const char *password = "P4npYfYS";

AsyncWebServer server(80);

//WiFiClient client;

void setup() {

  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);

  delay(2000);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected..!");

  delay(1000);

  server.on("/relaySwitch/", HTTP_GET, handle_relaySwitch);
//  server.onNotFound([](AsyncWebServerRequest *request) {
//    if (request->method() == HTTP_OPTIONS) {
//      request->send(200);
//    } else {
//      request->send(404);
//    }
//  });
  server.on("*", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
    request->send(200);
  });
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "DELETE, POST, GET, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Access-Control-Allow-Headers, Access-Control-Allow-Origin, Content-Type, Authorization, X-Requested-With");
  server.begin();
  
  send_request();

}

void loop() {
//  if(millis()-timer >= 3000){
//    send_request();
//    timer = millis();
//  }
}


void handle_relaySwitch(AsyncWebServerRequest *request){
  
//  String receivedValue;

  if (request->hasParam("button_reading")){
    relayStatus = request->arg("button_reading");
  }
  Serial.print("Received value:");
  Serial.println(relayStatus);

  digitalWrite(relayPin, relayStatus.toInt());

  request->send(200, "text/plain", relayStatus);
  
}

void send_request(){
//  char serverHost[14] = "192.168.0.100";
//  const char * host = serverHost;
//  if(!client.connect(host, 80)){
//    Serial.print("Connection to host ");
//    Serial.print(host);
//    Serial.println(" failed");
//    return;
//    
//  }else{
//    Serial.print(host);
//    Serial.println(" Connection established");
//
//    client.print("GET /relayData/?data=" + String(12) + " HTTP/1.1");
//
//  }

  WiFiClient client;

  char serverHost[14] = "192.168.0.100";
  const char * host = "192.168.0.100";
  if(!client.connect(host, 80)){
    Serial.print("Connection to host ");
    Serial.print(host);
    Serial.println(" failed");
    return;
    
  }else{
    Serial.print(host);
    Serial.println(" Connection established");

  }

//  \"relay_name\":%20"%20+%20String(relayName)%20+%20",%20\"relay_status\":%20"%20+%20String(relayStatus)%20+%20",%20\"relay_ip\":%20"%20+%20String(relayIP)%20+%20"}%20HTTP/1.1

  String url = "/updateRelays/?relayData={\"relay_name\":\"relayName\",\"relay_status\":\"relayStatus\",\"relay_ip\":\"relayIp\"}";
  url.replace("relayName", relayName);
  url.replace("relayStatus", relayStatus);
  url.replace("relayIp", relayIP);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             "Connection: close\r\n\r\n");
             
  client.stop();
  
}

//http://192.168.0.101/relaySwitch/?button_reading=0
