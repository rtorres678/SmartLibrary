/****************************************
Example Sound Level Sketch for the 
Adafruit Microphone Amplifier
****************************************/
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

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
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level

   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
   //unsigned int samples=0;

   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      delayMicroseconds(14);
      //samples++;
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
      //if (samples > 400)break;
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

   if (peakToPeak > 0)
   {
    Serial.println(String(peakToPeak));
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      StaticJsonDocument<40> JSONbuffer;
      JSONbuffer["Reading"] = String(peakToPeak);
      String JSONoutput = "";
      serializeJson(JSONbuffer, JSONoutput);
      //http.begin("http://httpbin.org/post");   //Destination address:port/path
      http.begin("http://192.168.1.109:3000/");   
      http.addHeader("Content-Type", "application/json");

      int httpCode = http.POST(JSONoutput);
      String payload = http.getString();

      //Serial.println(httpCode);
      //Serial.println(payload);
      //Serial.println(samples);

      http.end();
    }else{
      Serial.println("Error in WiFi connection");
    }
   }
}
