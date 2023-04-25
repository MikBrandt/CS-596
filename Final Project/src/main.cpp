#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HttpClient.h>

#define BUZZTIMEMS 5000

/* FOR CLOUD CONNECTION */
char ssid[] = "Michael";    // your network SSID (name) 
char pass[] = "123456789aztecs"; // your network password (use for WPA, or use as key for WEP)

// Location of the server we want to connect to
const char kHostname[] = "34.209.250.108";
const int kPort = 5000;
// Path to download (this is the bit after the hostname in the URL
// that you want to download)
char kPath[100] = "/?motion=detected";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

/* FOR PIR SENSOR */
int buzzerPin = 25;              
int inputPin = 26;            
int val = 0;
bool active = false;
unsigned long timeActiveStart;

void setup() {
    Serial.begin(9600);  
    pinMode(buzzerPin, OUTPUT);      
    pinMode(inputPin, INPUT);
    timeActiveStart = millis();

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

void connectToCloud(){
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
}

void loop(){

    // Read PIR sensor
    val = digitalRead(inputPin); 

    // turn off buzzer if 5 seconds passed since turning on
    if (millis() - timeActiveStart > BUZZTIMEMS && active){
        active = false;
        analogWrite(buzzerPin, 0);
        Serial.println("Buzzer off");
    }

    //if buzzer off and motion detected, turn it on
    if (val == HIGH && !active)  
    {            
        analogWrite(buzzerPin, 200);  
        active = true;
        timeActiveStart = millis();
        Serial.print("Buzzer on! ");
        Serial.println(timeActiveStart);
        connectToCloud();
    } 
    delay(1000);
}
