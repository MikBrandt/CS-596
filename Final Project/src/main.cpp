#include <Arduino.h>
#include <Wire.h>

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
} 

void loop(){

    // Read PIR sensor
    val = digitalRead(inputPin); 

    // turn off buzzer if 10 seconds passed since turning on
    if (millis() - timeActiveStart > 10000){
        active = false;
        analogWrite(buzzerPin, 0);
        Serial.println("Buzzer off");
    }

    //if buzzer off and motion detected, turn it on
    if (val == HIGH && !active)  
    {            
        analogWrite(buzzerPin, 200);  
        Serial.println("Buzzer on!"); 
        active = true;
        timeActiveStart = millis();
    } 
    delay(10);
}