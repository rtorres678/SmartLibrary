/****************************************
Smart Library dB Meter sketch
Reads microphone and sends volume measurements to webserver via HTTP POST request
Credit Bill Earl: https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
****************************************/
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
const String macAddr = WiFi.macAddress();

void setup() 
{
   Serial.begin(115200);
   WiFi.begin("Cisco23558", "ferret42");
   
   while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for connection");
   }
}


void loop() 
{
   unsigned long startTime = millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data for 50 mS
   while (millis() - startTime < sampleWindow)
   {
      sample = analogRead(0);
      delayMicroseconds(14);  //Required to avoid dropping WiFi connection
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   
   if (peakToPeak > 0)  //Set bottom threshold
   {
    Serial.println(String(peakToPeak));
    //Serial.println(macAddr);
    if(WiFi.status() == WL_CONNECTED){
      HTTPClient http;

      //Prepare JSON to send in POST request
      StaticJsonDocument<100> JSONbuffer;
      JSONbuffer["Val"] = String(peakToPeak);
      JSONbuffer["ID"] = macAddr;
      String JSONoutput = "";
      serializeJson(JSONbuffer, JSONoutput);

      //Begin HTTP connection
      http.begin("http://192.168.1.109:3000/");   //Destination address:port/path
      http.addHeader("Content-Type", "application/json");

      int httpCode = http.POST(JSONoutput); //Should return 200 if OK
      String payload = http.getString();  //Contains response from the server

      //Serial.println(httpCode);
      //Serial.println(payload);
      //Serial.println(samples);

      http.end();
    }else{
      Serial.println("Error in WiFi connection");
    }
   }
}
