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
String displayMsg = "0";
unsigned int requestsCount = 0;

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

//JSON that stores relays/devices data
StaticJsonDocument<400> relaysJSON;


void setup() {

  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //Initialize display
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
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  display.println("");
  display.print("WiFi connected!");
  display.display();
  delay(1000);

 // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
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

  //Relays data request from server webpage
  server.on("/getRelaysData", HTTP_GET, handle_getRelaysData);

  //Receive relay data from another ESP device
  server.on("/updateRelays", HTTP_GET, handle_updateRelays);

  server.begin();
  
  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("Server started!");
  display.display();

}

void loop() {
  if(displayMsg == "0"){
    return;
  }
  
  if(displayMsg == "deviceRequestMsg"){
    display.clearDisplay();
    display.setCursor(0, 5);
    display.println("Server is running");
    display.println("- Hadling device request...");
    display.display();
    return;
  }

  if(displayMsg == "webpageRequestMsg"){
    display.clearDisplay();
    display.setCursor(0, 5);
    display.println("Server is running");
    display.print("- Handling webpage request...");
    display.print("(" + String(requestsCount) + " call(s))");
    display.display();
    return;
  }
}

void handle_updateRelays(AsyncWebServerRequest *request){

  Serial.print("Hadling device request...");
  
  displayMsg = "deviceRequestMsg";
  
  String relayData = "";

  if (request->hasParam("relayData")){
    relayData = request->arg("relayData");
  }
  
  Serial.print("Received value:");
  Serial.println(relayData);

  //Pull IP adress from received device with JSON
  DynamicJsonDocument receivedRelay(1024);
  deserializeJson(receivedRelay, relayData);
  String relayIP = receivedRelay["relay_ip"];

  //Find out if we alreadyhave device with this IP adress
  const char* hasRelay = relaysJSON[{}]["relay_ip"];
  
  if (String(hasRelay) == relayIP) {
    Serial.println("Already contains relay with ip: " + String(hasRelay) + ", returning...");
    return;
  }

  //Put new received relay data in JSON object
  JsonObject relay  = relaysJSON.createNestedObject();
  relay["relay_ip"] = receivedRelay["relay_ip"];
  relay["relay_name"] = receivedRelay["relay_name"];
  relay["relay_status"] = receivedRelay["relay_status"];
  serializeJson(relaysJSON, Serial);
  Serial.println();

  request->send(200, "text/plain", "ok");  
}


void handle_getRelaysData(AsyncWebServerRequest *request){

  Serial.println("Handling webpage data request...");
  serializeJson(relaysJSON, Serial);
  Serial.println();

  displayMsg = "webpageRequestMsg";
  
  String receivedData = "";
  serializeJson(relaysJSON, receivedData);
  
  request->send(200, "text/plain", receivedData);
  
  requestsCount++;
  
}


//http://192.168.0.101/relaySwitch/?button_reading=0
