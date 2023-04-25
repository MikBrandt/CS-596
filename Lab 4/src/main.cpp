#include <Arduino.h>
#include <Wire.h>
#include "SparkFunLSM6DSO.h"
#include <BLEDevice.h>
#include <string>
#include <algorithm>
#include <cmath>


#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define LEDPIN 17
#define CALLIBRATIONMS 5000

bool calibrated = false;
unsigned long start_callibration_time;
float rootMeanSquare;
float threshhold = 0;
unsigned int steps = 0;
unsigned int prevSteps_TMP = 0;

LSM6DSO myIMU;
bool stepping = false;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue().c_str();
        if (value == "On" || value == "on"){digitalWrite(LEDPIN, HIGH);}
        else if (value == "Off" || value == "off"){digitalWrite(LEDPIN, LOW);}
    }
};

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

void setup()
{
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  Serial.begin(9600);
  delay(500);

  Wire.begin();
  if( myIMU.begin() )
    Serial.println("Ready.");
  else { 
    Serial.println("Could not connect to IMU.");
    Serial.println("Freezing");
    while(1);
  }

  if( myIMU.initialize(BASIC_SETTINGS) )
    Serial.println("Settings Loaded.");
  
  Serial.println("Starting BLE work!");
  
  BLEDevice::init("SDSUCS");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                          CHARACTERISTIC_UUID,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE |
                                          BLECharacteristic::PROPERTY_NOTIFY
                                        );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Server Example -- SDSU IOT");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void calibrate(){
  Serial.println("Calibrating...");
  while (millis() - start_callibration_time < CALLIBRATIONMS){
    rootMeanSquare = float(sqrt(pow(myIMU.readFloatGyroX(), 2) + 
                            pow(myIMU.readFloatGyroY(), 2)+
                            pow(myIMU.readFloatGyroX(), 2)));
    threshhold = std::max(threshhold, rootMeanSquare);
    
  }
  calibrated = true;
  Serial.println("Finished Calibrating");
}

void loop()
{
  if (!calibrated){
    start_callibration_time = millis();
    calibrate();
  }
  rootMeanSquare = float(sqrt(pow(myIMU.readFloatGyroX(), 2) + 
                          pow(myIMU.readFloatGyroY(), 2)+
                          pow(myIMU.readFloatGyroX(), 2)));
  if (rootMeanSquare > threshhold){
      if (!stepping){
        steps++;
        stepping = true;
        delay(500);
      }
  }
  else{stepping = false;}

  if (prevSteps_TMP < steps){
    Serial.print(steps);
    Serial.println(" steps");
    prevSteps_TMP = steps;
    pCharacteristic->setValue(steps);
    pCharacteristic->notify(); // This line sends the updated value to the client device
    // The value is sent in hex format
  }
  delay(20);
}
