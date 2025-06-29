#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "YourTemplateName"
#define BLYNK_AUTH_TOKEN "YourAuthToken"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>

// Replace with your WiFi credentials
char ssid[] = "YourWiFiSSID";
char pass[] = "YourWiFiPassword";

// Hardware Pins
#define PIR_PIN 13
#define BUZZER_PIN 14
#define PANIC_BTN_PIN 26

// Default Active Window
int startHour   = 11;
int startMinute = 30;
int endHour     = 11;
int endMinute   = 32;

const int PRE_ALERT_OFFSET = 1;
const int MOTION_THRESHOLD = 1;

int motionCount = 0;
int buzzerPeriodCount = 0;
bool alertSent = false, resetDone = false;
bool buzzerManual = false;
bool buzzerAutoActive = false;

unsigned long lastCheck = 0;

int toMinutes(int h, int m) { return h * 60 + m; }
int currentTime() { return toMinutes(hour(), minute()); }

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(PANIC_BTN_PIN, INPUT_PULLUP);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected");

  Blynk.sendInternal("rtc", "sync");
  Serial.println("System Ready");
}

BLYNK_WRITE(InternalPinRTC) {
  setTime(param.asLong());
}

BLYNK_WRITE(V2) { startHour = param.asInt(); }
BLYNK_WRITE(V3) { startMinute = param.asInt(); }
BLYNK_WRITE(V4) { endHour = param.asInt(); }
BLYNK_WRITE(V5) { endMinute = param.asInt(); }

BLYNK_WRITE(V0) {
  buzzerManual = (param.asInt() == HIGH);
}

void checkMotion() {
  bool motionDetected = digitalRead(PIR_PIN);
  if (motionDetected && (millis() - lastCheck > 2000)) {
    motionCount++;
    lastCheck = millis();
    if (buzzerAutoActive) {
      buzzerPeriodCount++;
      Blynk.virtualWrite(V1, buzzerPeriodCount);
    }
  }
}

void loop() {
  Blynk.run();

  int now   = currentTime();
  int start = toMinutes(startHour, startMinute);
  int end   = toMinutes(endHour, endMinute);
  int alertTime = end - PRE_ALERT_OFFSET;

  if (hour() == startHour && minute() == startMinute && !resetDone) {
    motionCount = 0;
    buzzerPeriodCount = 0;
    alertSent = resetDone = buzzerAutoActive = false;
    if (!buzzerManual) digitalWrite(BUZZER_PIN, LOW);
  }
  if (!(hour() == startHour && minute() == startMinute)) resetDone = false;

  if (now >= start && now <= end) {
    checkMotion();
  }

  if (now == alertTime && !buzzerAutoActive && motionCount < MOTION_THRESHOLD && !buzzerManual) {
    buzzerAutoActive = true;
    Blynk.virtualWrite(V1, "Buzzer: ACTIVE (Auto)");
  }

  if (now == end) {
    if (buzzerAutoActive) {
      buzzerAutoActive = false;
      if (!alertSent && buzzerPeriodCount < MOTION_THRESHOLD) {
        Blynk.logEvent("motion_notifier", "Low Motion Detected");
        alertSent = true;
      }
    }
  }

  static bool lastBtnState = HIGH;
  static unsigned long lastDebounceTime = 0;
  bool currentBtnState = digitalRead(PANIC_BTN_PIN);
  if (currentBtnState != lastBtnState && (millis() - lastDebounceTime) > 50) {
    lastDebounceTime = millis();
    lastBtnState = currentBtnState;
    if (currentBtnState == LOW) {
      Blynk.logEvent("panic_button", "Panic Button Activated");
    }
  }

  if (buzzerAutoActive && digitalRead(PIR_PIN) == HIGH) {
    buzzerAutoActive = false;
    digitalWrite(BUZZER_PIN, LOW);
  }

  static unsigned long previousBlinkTime = 0;
  unsigned long currentMillis = millis();
  unsigned long blinkInterval = 0;

  if (buzzerManual) {
    blinkInterval = 1000;
  } else if (buzzerAutoActive) {
    blinkInterval = 500;
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    previousBlinkTime = currentMillis;
    delay(10);
    return;
  }

  if (currentMillis - previousBlinkTime >= blinkInterval) {
    int currentState = digitalRead(BUZZER_PIN);
    digitalWrite(BUZZER_PIN, !currentState);
    previousBlinkTime = currentMillis;
  }

  delay(10);
}
