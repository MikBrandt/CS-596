#include <Arduino.h>

#define RED_PIN 32
#define YELLOW_PIN 33
#define GREEN_PIN 25
#define BUZZER_PIN 26
#define BUTTON_READ_PIN 37

// Hard coded enumerator
#define RED_STATE 0
#define RED_YELLOW_STATE 1
#define YELLOW_STATE 2
#define GREEN_STATE 3

#define RED_MILLIS 10000

int tl_state;           // Traffic light state.
unsigned long tl_timer; // Traffic light timer.
unsigned long buzzer_timer; // buzzer timer
unsigned long switch_time; // time state was switched
bool waiting = false; // waiting to turn yellow after button is pressed
void setup()
{
    // Configure LED pins as outputs. 
    pinMode(RED_PIN, OUTPUT); 
    pinMode(YELLOW_PIN, OUTPUT); 
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_READ_PIN, INPUT);

    // Initial state for states and timers.. 
    tl_state = GREEN_STATE;
    Serial.begin(9600);
}

void loop()
{   
    int buttonPressed = !digitalRead(BUTTON_READ_PIN); // 1 means button is pressed
    buzzer_timer = millis() % 2000; // buzzer cycles in either 500ms on & 1500ms off or 250ms on & 150ms off, so 2000ms is the least common factor
    // TL state machine
    switch (tl_state)
    {
    case RED_STATE:
        //control buzzer
        if (buzzer_timer%500 <= 250){analogWrite(BUZZER_PIN, 200);} // 250ms on, 250ms off
        else{analogWrite(BUZZER_PIN, 0);}
        // Turn red light on.
        digitalWrite(RED_PIN, HIGH);
        if (millis() - switch_time > 10000){ // remain in red state for 10 seconds
          tl_state = RED_YELLOW_STATE; // next state is red-yellow
          switch_time = millis();
        }
        break;
    case RED_YELLOW_STATE:
        //control buzzer
        if (buzzer_timer%500 <= 250){analogWrite(BUZZER_PIN, 200);} // 250ms on, 250ms off
        else{analogWrite(BUZZER_PIN, 0);}
        // Code for red-yellow state.
        digitalWrite(YELLOW_PIN, HIGH); // red-yellow state described by having red and yellow LEDs on
        if (millis() - switch_time > 2000){ // remain in yellow-red state for 2 seconds
          digitalWrite(RED_PIN, LOW);
          digitalWrite(YELLOW_PIN, LOW);
          tl_state = GREEN_STATE; // next state is green
        }
        break; 
    case YELLOW_STATE:
        //control buzzer
        if (buzzer_timer <= 500){analogWrite(BUZZER_PIN, 200);} // 500ms on, 1500ms off
        else{analogWrite(BUZZER_PIN, 0);}
        // Code for yellow state.
        digitalWrite(YELLOW_PIN, HIGH);
        if (millis() - switch_time > 2000){ // remain in yellow state for 2 seconds
          digitalWrite(YELLOW_PIN, LOW);
          tl_state = RED_STATE; // next state is red state
          switch_time = millis();
        }
        break;
    case GREEN_STATE:
        // Turn green light on.
        digitalWrite(GREEN_PIN, HIGH);
        //control buzzer
        if (buzzer_timer <= 500){analogWrite(BUZZER_PIN, 200);} // 500ms on, 1500ms off
        else{analogWrite(BUZZER_PIN, 0);}
        //check for button press
        if (buttonPressed && !waiting){ // A button press is only registered if we are in the green state and not about to switch states
          waiting=true; // declare that we are waiting 5 seconds to switch to yellow state
          tl_timer = millis();
          }
        // wait 5 seconds after button press to switch states
        if (waiting && (millis() - tl_timer > 5000))
        {
            digitalWrite(GREEN_PIN, LOW);
            tl_state = YELLOW_STATE; // next state is yellow
            switch_time = millis();
            waiting=false; // we are no longer waiting to switch to yellow state, so next time we are in the green state we are ready for a button press again
        }
        break;
    }
}