#include <Arduino.h>

const int buttonPin = 21;                       
const int ledPin =  2;                        
int buttonState = 0;                        
void setup() {
    pinMode(ledPin, OUTPUT);                  
    pinMode(buttonPin, INPUT_PULLUP);          
}                                               
void loop() {
   buttonState = digitalRead(buttonPin);       
   if (buttonState == HIGH) {                 
       digitalWrite(ledPin, HIGH);            
   } else {
       digitalWrite(ledPin, LOW);              
   }
}