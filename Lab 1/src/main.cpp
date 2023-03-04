#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>

// set mode to I2C or SPI
char* mode = "spi";

// initialize tft, cap objects and count variable
TFT_eSPI tft; // for printing to screen
Adafruit_CAP1188 cap; // for communicating with touch sensor
int count; // counting number of pins touched

// pins used for SPI communication
#define CAP1188_MOSI  GPIO_NUM_32
#define CAP1188_MISO  GPIO_NUM_33
#define CAP1188_CLK  GPIO_NUM_25
#define CAP1188_CS  GPIO_NUM_26
#define CAP1188_RESET  GPIO_NUM_13

void setup_i2c(){
  cap = Adafruit_CAP1188();
  Wire.begin();
}

void setup_spi(){
  cap = Adafruit_CAP1188(CAP1188_CLK, CAP1188_MISO, CAP1188_MOSI, CAP1188_CS, CAP1188_RESET);
  pinMode(CAP1188_CLK, OUTPUT);
  pinMode(CAP1188_MOSI, OUTPUT);
  pinMode(CAP1188_CS, OUTPUT);
  pinMode(CAP1188_RESET, OUTPUT);
  pinMode(CAP1188_MISO, INPUT);
}

void setup() {
  if (mode == "i2c"){
    setup_i2c();
  }
  else{
    setup_spi();
  }
  Serial.begin(9600); // for debugging
  tft.begin(); // for the screen
  tft.fillScreen(0); // make screen black
  tft.setTextSize(2); // make text larger
  tft.setTextDatum(MC_DATUM); // center text
  tft.setRotation(135); // rotate text so its horizontal
  tft.drawString("Pins touched:", tft.width() / 2, tft.height() / 2); // Write the static part
  cap.begin(); // for the CAP1188
}

void loop() {
  uint8_t touched = cap.touched(); // get number of touched pins (0 bit = not touched, 1 bit = touched)
  count = 0;
  for (uint8_t i=0; i<8; i++) { // loop through each bit
    if (touched & (1 << i)) { // if a bit it 1, increment counter
      count++;
    }
  }
  tft.drawNumber(count, tft.width() / 2, tft.height() / 2 + 25); // print count to screen
  delay(50);
}