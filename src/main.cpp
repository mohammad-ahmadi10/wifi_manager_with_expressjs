#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


// ssid and password, they are be written by readFile
String ssid;
String password;


// local IP and Access point IP
const char* WIFI_IP = "192.168.2.24";
const char* AP_IP = "192.168.2.22";

// Timer variables
unsigned long previousMillis = 0;
const long interval = 6000000;  // interval to wait for Wi-Fi connection (milliseconds)

  
  // configrate IP network 
  //IPAddress local_IP(192 , 168 , 1, 22);
  IPAddress localIP;
  IPAddress gateway(192 , 168 , 1, 5);
  IPAddress subnet(250 ,250 , 250 , 0);
  
  // Creating sever
  AsyncWebServer server(80);


// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/password.txt";


// initilizing the WiFi
bool initWiFi() {
  if(ssid==""){
    Serial.println(F("Undefined SSID or password address."));
    return false;
  }


  // configuring WIFI network 
  WiFi.mode(WIFI_STA);
  localIP.fromString(WIFI_IP);
  if (!WiFi.config(localIP, gateway, subnet)){
    Serial.println(F("STA Failed to configure"));
    return false;
  }
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println(F("Connecting to WiFi..."));

  previousMillis = 0;
  while(WiFi.status() != WL_CONNECTED) {
    previousMillis +=1;
    if (previousMillis >= interval) {
      Serial.println(F("Failed to connect."));
      return false;
    }

  }

  Serial.print(F("Connection was Successfull!  connect to your Router and go to the IP: "));
  Serial.println(WiFi.localIP());
  Serial.println(F("go to /config in order to reconfonfig the Wlan!"));
  return true;
}


// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println(F("- failed to open file for writing"));
    return;
  }
  if(file.print(message)){
    Serial.println(F("- file written"));
  } else {
    Serial.println(F("- frite failed"));
  }
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println(F("- failed to open file for reading"));
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// get the data
void getIndexPage(const char* url){      
      server.on(url  , HTTP_GET , [](AsyncWebServerRequest *request){
        request->send(SPIFFS , "/index.html" , "text/html" , false );
      });
      server.serveStatic("/index.html" , SPIFFS , url);

      server.on("/favicon.ico" , HTTP_GET , [](AsyncWebServerRequest *request){
        request->send(SPIFFS , "/favicon.png" , "image/png");
      });
       server.serveStatic("/favicon.png" , SPIFFS , "/favicon.ico");

      server.on("/wifi_icon.svg" , HTTP_GET , [](AsyncWebServerRequest *request){
          request->send(SPIFFS , "/wifi_icon.svg" , "image/svg+xml");
      });


      server.on("/style.css" , HTTP_GET , [](AsyncWebServerRequest *request){
          request->send(SPIFFS , "/style.css" , "text/css");
      });
      server.on("/script.js" , HTTP_GET , [](AsyncWebServerRequest *request){
          request->send(SPIFFS , "/script.js" , "text/javascript");
      });


}
// posting the data
void PostData(const char* url){
  server.on(url, HTTP_POST, [](AsyncWebServerRequest *request) {
        int params = request->params();

        for(int i=0;i<params;i++){
          AsyncWebParameter* p = request->getParam(i);
          if(p->isPost()){
            if (p->name() == "ssid") {
              ssid = p->value().c_str();
              Serial.print("SSID set to: ");
              Serial.println(ssid);
              writeFile(SPIFFS ,ssidPath ,  ssid.c_str());
            }

            if (p->name() == "password") {
              password = p->value().c_str();
              Serial.print("password set to: ");
              Serial.println(password);
              writeFile(SPIFFS , passPath , password.c_str());
            }            
          }
        }
        request->send(200 , "text/plain" ,
            "The network Data is secussfully send , ESP restarts , please check the ESP wheater the Connection was sucessfull!"); 
        delay(3000);
        ESP.restart();
      });
      server.begin();

}

void setup() {

  Serial.begin(115200);

  if ( !SPIFFS.begin()){
    Serial.println(F("An Error has occurred while mounting SPIFFS!"));
    return;
  }

  ssid = readFile(SPIFFS, ssidPath);
  password = readFile(SPIFFS, passPath);

  // checking if the esp is connected to the wifi, if not create the AP for configuration
  if(initWiFi()){

      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send( 200 , "text/plain" , "successfull!!!!!!");
      });

      PostData("/data");
      getIndexPage("/config");
      server.begin();
  }else{

    WiFi.mode(WIFI_AP);
    localIP.fromString(AP_IP);
    Serial.print(F("Setting up Access Point ..."));
    Serial.println(F(WiFi.softAPConfig(localIP , gateway , subnet) ? "ready" : "failed!"));
    Serial.println(F(WiFi.softAP("ESP32" , NULL) ? "Ready" : "Failed!"));

    Serial.print(F("Connecto to the ESP32 AP in order to config Wlan and go to the Address IP: "));
    Serial.println( WiFi.softAPIP() );

      getIndexPage("/");
      PostData("/data");
      server.begin();
  }
}
void loop() {}