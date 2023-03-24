#include <WiFi.h>
#include <HttpClient.h>
#include <Arduino.h>
#include "DHT20.h"

#define DHTPIN GPIO_NUM_25
#define DHTTYPE 20

DHT20 dht; // instance of humidity/temp sensor (DHT 20)

char ssid[] = "Michael";    // your network SSID (name) 
char pass[] = "123456789"; // your network password (use for WPA, or use as key for WEP)

// Location of the server we want to connect to
const char kHostname[] = "34.235.150.78";
const int kPort = 5000;
// Path to download (this is the bit after the hostname in the URL
// that you want to download)
char kPath[100];

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void setup() {
  Serial.begin(9600);
  dht.begin();

  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  dht.read(); // read temp and humidity from sensor
  float temp = dht.getTemperature();  // get temp
  float humidity = dht.getHumidity(); // get humidity
  sprintf(kPath, "/?temperature=%.2f&humidity=%.2f", temp, humidity); // store temp and humidity in path variable for server to read

  int err =0;
  WiFiClient c;
  HttpClient http(c);
  
  err = http.get(kHostname, kPort, kPath);
  if (err == 0)
  {
    Serial.println("");
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();

  // Sleep for 10 seconds
  sleep(10);
}