/*
  Author: Sandeep Tembare
  
  The code will Enable OTA update to ESP8266 with Telnet.
  The code will print the same data to Serial and Telnet Console.
  
  Simply telnet to IP of Your ESP and check data status.
  Use data_print and data_println to print data.
  
  Telnet Library used: https://github.com/jandrassy/TelnetStream
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>

#ifndef STASSID
#define STASSID ""            //Set your SSID here
#define STAPSK  ""            //Set Your Password Here
#endif
#define LED 2                 //Define LED Pin

const char* ssid = STASSID;
const char* password = STAPSK;

//Printing data to Serial and Telnet
void data_print(String information){
  Serial.print(information);
  TelnetStream.print(information);  
}

void data_println(String information){
  Serial.println(information);
  TelnetStream.println(information);  
}

//OTA Initialization
void ota_setup(){
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Sandeep-ESP8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    data_println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    data_println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    data_print("Progress: %u%%\r"+ String(progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    data_print("Error[%u]: "+ String(error));
    if (error == OTA_AUTH_ERROR) {
      data_println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      data_println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      data_println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      data_println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      data_println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void setup() {
  
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  data_println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    data_println("Connection Failed! Rebooting...");
    digitalWrite(LED, HIGH);
    delay(5000);
    ESP.restart();
  }
  TelnetStream.begin();
  digitalWrite(LED, LOW);

  ota_setup();
  data_println("Ready");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());

}

void loop() {
  ArduinoOTA.handle();

  //To reset code ESP. Send "R" via telnet
  switch (TelnetStream.read()) {
    case 'R':
    TelnetStream.stop();
    delay(100);
    ESP.reset();
      break;
  }

  //Your Code Here
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  data_println("LED ON");
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  data_println("LED OFF");
  delay(1000);                       // wait for a second
  
}
