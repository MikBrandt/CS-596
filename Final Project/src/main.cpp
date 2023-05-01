#include <Arduino.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define BUZZTIMEMS 15000
#define SLEEPTIMESECS 20
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

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
  
    BLEDevice::init("SDSUCS");
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE |
                                            BLECharacteristic::PROPERTY_NOTIFY
                                            );
    pCharacteristic->setValue("Server Example -- SDSU IOT");
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x0); 
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    pCharacteristic->notify();
    Serial.println("Starting BLE work!");
} 

void loop(){

    // Read PIR sensor
    val = digitalRead(inputPin); 

    // turn off buzzer if 5 seconds passed since turning on
    if ((millis() - timeActiveStart > BUZZTIMEMS) && active){
        active = false;
        Serial.println("Buzzer off");
        analogWrite(buzzerPin, 0);
        sleep(SLEEPTIMESECS);
        BLEDevice::startAdvertising();
    }

    //if buzzer off and motion detected, turn it on
    if (val == HIGH && !active)  
    {   
        active = true;
        timeActiveStart = millis();
        Serial.println("Buzzer on!");
        pCharacteristic->setValue("motion");
        pCharacteristic->notify();
        delay(1000);
        pCharacteristic->setValue("no motion");
        pCharacteristic->notify();
    }
    if ((millis() % 500 < 250) && active){
        analogWrite(buzzerPin, 200);
    }
    else {
        analogWrite(buzzerPin, 0);
    }
    delay(100);
}
