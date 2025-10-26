//Assignment 1 Part B
// long press buzzer
//Embeded IOT FALL -2025
// Husnain Nadeem                            //REG 23-NTU-CS-1038
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin configuration
const int buttonPin = 32;  // single button
const int ledPin = 16;     // LED pin
const int buzzerPin = 17;  // buzzer pin

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables for button timing
unsigned long buttonPressTime = 0;
bool isButtonPressed = false;
bool ledState = false;
bool longPressActive = false;
const unsigned long longPressDuration = 1500; // 1.5 seconds for long press
const unsigned long debounceDelay = 50;       // 50ms debounce time

void showOLED(const char *message) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Wire.begin(21, 22); // SDA = 21, SCL = 22 for ESP32
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED init failed");
    while (true);
  }
  showOLED("Ready");
  
  // Initialize LED to OFF state
  digitalWrite(ledPin, LOW);
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  // Button pressed (LOW because of INPUT_PULLUP)
  if (buttonState == LOW && !isButtonPressed) {
    // Wait for debounce period to confirm the press
    delay(debounceDelay);
    // Check button again after debounce
    if (digitalRead(buttonPin) == LOW) {
      isButtonPressed = true;
      buttonPressTime = millis();
      longPressActive = false;
      Serial.println("Button pressed - waiting for release");
    }
  }

  // Button is being held
  if (buttonState == LOW && isButtonPressed) {
    if (!longPressActive && (millis() - buttonPressTime > longPressDuration)) {
      // Long press detected – activate buzzer
      longPressActive = true;
      tone(buzzerPin, 1000);
      showOLED("BUZZER");
      Serial.println("Long press activated - BUZZER ON");
    }
  }

  // Button released (HIGH because of INPUT_PULLUP)
  if (buttonState == HIGH && isButtonPressed) {
    // Wait for debounce period to confirm the release
    delay(debounceDelay);
    // Check button again after debounce
    if (digitalRead(buttonPin) == HIGH) {
      noTone(buzzerPin); // stop buzzer immediately when released

      if (!longPressActive) {
        // Short press – toggle LED
        ledState = !ledState;
        digitalWrite(ledPin, ledState ? HIGH : LOW);
        showOLED(ledState ? "LED ON" : "LED OFF");
        Serial.println(ledState ? "LED turned ON" : "LED turned OFF");
      } else {
        // If long press happened
        showOLED("Stopped");
        Serial.println("Long press stopped");
      }

      // Reset state
      isButtonPressed = false;
      longPressActive = false;
    }
  }
}