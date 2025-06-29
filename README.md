# ESP32 Motion Notifier with Buzzer (Blynk IoT Project)

This project uses an ESP32 microcontroller to detect motion through a PIR sensor and trigger a buzzer alert based on time configurations. The system is connected to the Blynk IoT platform for real-time monitoring, configuration, and alerts.

## 🚀 Features

- 🔔 Buzzer alert for low motion activity
- ⏰ Customizable active time window from Blynk
- 📲 Manual buzzer activation from app
- 🚨 Panic button for emergency alert
- 🔄 Daily reset of counters
- 📉 Low motion notification sent automatically via Blynk

## 🧰 Components Used

| Component           | Description                      |
|--------------------|----------------------------------|
| ESP32 Board        | Microcontroller                  |
| PIR Sensor         | Motion Detection (GPIO 13)       |
| Buzzer             | Active Buzzer (GPIO 14)          |
| Push Button        | Panic Button (GPIO 26)           |
| WiFi               | Required for Blynk connection    |
| Blynk IoT App      | Real-time monitoring & control   |

## 📲 Blynk App Configuration

| Virtual Pin | Function             |
|-------------|----------------------|
| V0          | Manual Buzzer Toggle |
| V1          | Motion Count Display |
| V2 - V5     | Start/End Time Set   |

Ensure to set up your Blynk template and replace the following in the `.ino`:
```cpp
#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "YourTemplateName"
#define BLYNK_AUTH_TOKEN "YourAuthToken"
# esp32-motion-alert-buzzer-blynk
