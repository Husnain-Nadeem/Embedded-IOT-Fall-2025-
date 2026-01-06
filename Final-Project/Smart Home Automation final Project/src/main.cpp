#define BLYNK_TEMPLATE_ID "TMPL6FadPK77r"
#define BLYNK_TEMPLATE_NAME "Esp automation"
#define BLYNK_AUTH_TOKEN "nz18rqPKvIHa9sEKOQQ-wvKzvD7PKV5o"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// ---------- WIFI ----------
char ssid[] = "TECNO SPARK 5 Pro";
char pass[] = "hello123";

// ---------- GPIO ----------
#define RELAY_ROOM 14      // Room light
#define RELAY_TEMP 27      // Temperature relay
#define TRIG_PIN 22        // Ultrasonic TRIG
#define ECHO_PIN 23        // Ultrasonic ECHO
#define DHTPIN 26
#define DHTTYPE DHT11

// ---------- OBJECTS ----------
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ---------- STATES ----------
bool roomLight = false;     // Room light state
bool detectionLock = false; // To detect when object moves away

// ---------- BLYNK ----------
BLYNK_WRITE(V0) {
  roomLight = param.asInt();
  digitalWrite(RELAY_ROOM, roomLight ? LOW : HIGH);
  Serial.println(roomLight ? "☁️ Blynk → LIGHT ON" : "☁️ Blynk → LIGHT OFF");
}

// ---------- ULTRASONIC ----------
long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000); // 25ms timeout
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void checkUltrasonic() {
  long distance = readDistanceCM();
  if (distance < 0) return;

  Serial.print("📏 Distance: "); Serial.print(distance); Serial.println(" cm");

  // Toggle light when an object comes within 10cm and lock until it moves away
  if (distance <= 10 && !detectionLock) {
    roomLight = !roomLight;
    digitalWrite(RELAY_ROOM, roomLight ? LOW : HIGH);
    Blynk.virtualWrite(V0, roomLight);
    detectionLock = true;
    Serial.println(roomLight ? "💡 Light ON (Ultrasonic)" : "💡 Light OFF (Ultrasonic)");
  }

  // Reset detection lock when object moves away
  if (distance > 10 && detectionLock) {
    detectionLock = false;
  }
}

// ---------- DHT ----------
void readDHT() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) return;

  Serial.print("🌡 Temp: "); Serial.print(temp); Serial.print(" °C | 💧 Hum: "); Serial.println(hum);
  Blynk.virtualWrite(V2, temp);
  Blynk.virtualWrite(V3, hum);

  digitalWrite(RELAY_TEMP, (temp > 20) ? LOW : HIGH); // Temp relay logic
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Ultrasonic + DHT Smart System Booting...");

  pinMode(RELAY_ROOM, OUTPUT);
  pinMode(RELAY_TEMP, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(RELAY_ROOM, HIGH);  // Light OFF initially
  digitalWrite(RELAY_TEMP, HIGH);  // Temp relay OFF initially

  dht.begin();

  // -------- WIFI CONNECT --------
  Serial.print("📶 Connecting to WiFi: "); Serial.println(ssid);
  WiFi.begin(ssid, pass);

  int wifi_retry = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_retry < 20) {
    delay(1000);
    Serial.print(".");
    wifi_retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("IP Address: "); Serial.println(WiFi.localIP());

    Serial.println("🔑 Connecting to Blynk...");
    Blynk.config(BLYNK_AUTH_TOKEN);
    int blynk_retry = 0;
    while (!Blynk.connect() && blynk_retry < 10) {
      Serial.print(".");
      delay(1000);
      blynk_retry++;
    }

    if (Blynk.connected()) {
      Serial.println("\n✅ Blynk Connected!");
    } else {
      Serial.println("\n❌ Blynk Connection Failed!");
    }
  } else {
    Serial.println("\n❌ WiFi Failed to Connect!");
  }

  // Start timers
  timer.setInterval(2000L, readDHT);        // DHT sensor every 2 sec
  timer.setInterval(300L, checkUltrasonic); // Ultrasonic every 300 ms
}

// ---------- LOOP ----------
void loop() {
  Blynk.run();
  timer.run();
}
