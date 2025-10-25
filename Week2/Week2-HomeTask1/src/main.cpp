#include <Arduino.h>

const int buttonPin = 25;     
const int led1 = 18;        
const int led2 = 16;        

int buttonState = 0;         
int lastButtonState = 0;     
int ledToggle = 0;          

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, HIGH); 
  digitalWrite(led2, LOW);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    ledToggle = !ledToggle;  

    if (ledToggle == 0) {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
    } else {
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
    }
    delay(200); 
  }

  lastButtonState = buttonState;
}
