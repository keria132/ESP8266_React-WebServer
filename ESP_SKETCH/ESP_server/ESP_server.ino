#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

short relayPin = 14;
bool relayStatus = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 100);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

//SSID and Password to connect to wifi
const char *ssid = "Tobik_Hata";
const char *password = "P4npYfYS";

AsyncWebServer server(80);

//DynamicJsonDocument relaysJSON(1024);
StaticJsonDocument<400> relaysJSON;

void setup() {

  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 5);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
    display.println("STA Failed to configure!");
    display.display();
  }
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);

  display.println("Connecting to: ");
  display.print(ssid);
  display.display();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);

    display.print(".");
    display.display();
    
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  display.println("");
  display.print("WiFi connected!");
  display.display();
  delay(1000);

 // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
//    request->send(200, "text/plain", "hello");
  });

  // Route to load style.css file
  server.on("/index-48e69ebb.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index-48e69ebb.css", "text/css");
  });

  // Route to load style.css file
  server.on("/index-53285a5c.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index-53285a5c.js", "text/javascript");
  });

  // Route to IMAGES
  server.on("/lights__icon-505adce2.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/lights__icon-505adce2.svg");
  });

  server.on("/cloudy-d4db8331.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/cloudy-d4db8331.svg");
  });
  
  server.on("/heavy__rain-9cde5914.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/heavy__rain-9cde5914.svg");
  });
  
  server.on("/partly__cloudy-3abfda7e.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/partly__cloudy-3abfda7e.svg");
  });
  
  server.on("/rainy-e1acf387.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/rainy-e1acf387.svg");
  });
  
  server.on("/snowfall-1b95e795.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/snowfall-1b95e795.svg");
  });
  
  server.on("/sunny-c059fc74.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/sunny-c059fc74.svg");
  });
  
  server.on("/getRelaysData", HTTP_GET, handle_getRelaysData);
  
  server.on("/updateRelays", HTTP_GET, handle_updateRelays);

  server.begin();
  
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("Server started!");
  display.display();

}

void loop() {
  
//  if(millis()-timer >= 5000){
////    sendSensorData();
//    relayStatus = !relayStatus
//    timer = millis();
//  }
//  
//  display.clearDisplay();
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 10);
//  Display static text
//  display.println("I found asylum inside");
//  display.display();
//  delay(3000);
//  display.println("your armageddon eyes");
//  display.display();
//  delay(3000);
//  display.clearDisplay();
//  display.setCursor(0, 10);
//  display.println("I'd kill to kiss your apocalypse");
//  display.display();
//  delay(1000);
  
}

void handle_updateRelays(AsyncWebServerRequest *request){

  Serial.print("Hadling request...");
  String receivedRelay = "{}";

  if (request->hasParam("relayData")){
    receivedRelay = request->arg("relayData");
  }
  Serial.print("Received value:");
  Serial.println(receivedRelay);

  DynamicJsonDocument newRelay(1024);
  deserializeJson(newRelay, receivedRelay);
  String relayIP = newRelay["relay_ip"];
  
  const char* hasRelay = relaysJSON[{}]["relay_ip"];
  
  if (String(hasRelay) == relayIP) {
    Serial.println("Already contains relay with ip: " + String(hasRelay) + ", returning...");
    return;
  }
  
  JsonObject relay  = relaysJSON.createNestedObject();
  relay["relay_ip"] = newRelay["relay_ip"];
  relay["relay_name"] = newRelay["relay_name"];
  relay["relay_status"] = newRelay["relay_status"];
  serializeJson(relaysJSON, Serial);
  Serial.println();


  request->send(200, "text/plain", "ok");  
}


void handle_getRelaysData(AsyncWebServerRequest *request){

//  display.clearDisplay();
//  display.setCursor(0, 5);
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.println("Server started!");
//  display.println("Sending new status...!");
//  display.display();
  
//  String receivedData = "[{\"relay_name\": \"lights\", \"relay_status\": " + String(relayStatus) + ", \"relay_ip\": \"192.168.0.101\"}]";
  String receivedData = "";
  serializeJson(relaysJSON, receivedData);

//  Serial.println(request->url());
//  if (request->hasParam("relay_data")){
//    receivedData = request->arg("relay_data");
//  }
//  Serial.print("Received relay data:");
//  Serial.println(receivedData);

//  int value = receivedValue.toInt()

//  digitalWrite(relayPin, receivedValue.toInt());
//  if(relayPin == 1){
//    
//  }
  
  request->send(200, "text/plain", receivedData);
  
}

//http://192.168.0.101/relaySwitch/?button_reading=0
