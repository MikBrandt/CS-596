#include <Arduino.h>
#include <Servo.h>
#include <algorithm>

// declare global variables
Servo myservo;
int startTime;
int elapsedTime;
int light_min;
int light_max;
const int LED_PIN = 13;
int angle = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  startTime = millis();
  light_min = 4095;
  light_max = 0;
  myservo.attach(26);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // for the first 10 seconds, calibrate
  elapsedTime = millis() - startTime;
  int analog_read = analogRead(27); // reads analog input from pin 27
  if (elapsedTime <= 10000){
    light_min = std::min(analog_read, light_min); // calculate min analog value during calibration
    light_max = std::max(analog_read, light_max); // calculate max analog value during calibration

    // blink a light
    if (digitalRead(LED_PIN) == HIGH){
      digitalWrite(LED_PIN, LOW);
    }
    else{
      digitalWrite(LED_PIN, HIGH);
    }
    delay(500); // controls blink speed
  }

  // then, adjust servo angle
  else{
      analog_read = std::max(analog_read, light_min); // these two lines bound the signal between the min and max
      analog_read = std::min(analog_read, light_max); // values in case the calibration wasn't 100% accurate
      angle = int(180*double(analog_read - light_min)/(light_max - light_min)); // bound the angle between 0 and 180
      myservo.write(angle); // change servo angle
      delay(10); // add a short delay to not overload the servo
  }
}