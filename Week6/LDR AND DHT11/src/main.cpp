#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Pin configuration ---
#define LDR_PIN 34        // ADC pin for LDR (ESP32)
#define DHTPIN 14         // DHT22 data pin
#define DHTTYPE DHT11     // DHT22 sensor type

#define SDA_PIN 21        // I2C SDA
#define SCL_PIN 22        // I2C SCL

// --- OLED setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DHT sensor setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Timing (ms) ---
const unsigned long SHOW_DHT_MS = 3000; // show temp/humidity duration
const unsigned long SHOW_LDR_MS = 3000; // show LDR duration

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP32: DHT22 + LDR -> OLED");

  // Initialize I2C on custom pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;); // stop here
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Initialize DHT sensor
  dht.begin();

  delay(500);
}

void showDHTScreen(float temperature, float humidity) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Environment");

  display.setCursor(0, 14);
  display.setTextSize(2);
  // Print temperature with one decimal
  display.print(temperature, 1);
  display.print(" C");

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println("Humidity");

  display.setTextSize(2);
  display.setCursor(0, 48);
  display.print(humidity, 1);
  display.print(" %");

  display.display();
}

void showLDRScreen(int adcValue, float voltage) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Light (LDR)");

  display.setCursor(0, 14);
  display.setTextSize(2);
  display.print(adcValue);

  display.setTextSize(1);
  display.setCursor(0, 44);
  display.print("Voltage: ");
  display.print(voltage, 2);
  display.print(" V");

  display.display();
}

void loop() {
  // ----- Read DHT -----
  float temperature = dht.readTemperature(); // Celsius
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading DHT22 sensor!");
    // Show error briefly on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("DHT read error!");
    display.display();
    delay(SHOW_DHT_MS);
  } else {
    // Serial output
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" °C  |  Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");

    // OLED output
    showDHTScreen(temperature, humidity);
    delay(SHOW_DHT_MS);
  }

  // ----- Read LDR -----
  int adcValue = analogRead(LDR_PIN); // ESP32: 0 - 4095 (default 12-bit)
  // convert to voltage (assuming 3.3V ADC reference)
  float voltage = (adcValue / 4095.0) * 3.3;

  Serial.printf("LDR ADC: %d  |  Voltage: %.2f V\n", adcValue, voltage);

  // OLED output for LDR
  showLDRScreen(adcValue, voltage);
  delay(SHOW_LDR_MS);
}
