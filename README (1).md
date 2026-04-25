# 💊 CareNet — Smart Medication Reminder & Automated Dispensing System

> **Hackathon:** SYMBIOT | **Problem Statement:** SYMB0008  
> **Domain:** Embedded Systems & IoT | **Team:** CareNet

---

## 📌 Problem Statement

Medication adherence is a major challenge, especially for elderly individuals and patients with chronic illnesses. Missing doses or incorrect timing can lead to serious health complications.

CareNet is an intelligent embedded system that reminds users to take medication on time and automatically dispenses the correct dosage — with real-time camera monitoring, alerts, schedule tracking, and reliable real-world operation.

---

## ✨ Features

| Feature | Status |
|--------|--------|
| RTC-based scheduled dispensing | ✅ |
| Servo motor automated dispensing | ✅ |
| LCD real-time clock + status display | ✅ |
| Buzzer alert on dose time | ✅ |
| Manual override button | ✅ |
| Blynk IoT mobile app integration | ✅ |
| NTP time sync (IST) | ✅ |
| Last dose & next dose tracking on LCD | ✅ |
| **ESP32-CAM live monitoring stream** | ✅ |
| **Remote visual verification via camera** | ✅ |

---

## 🛠️ Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Main Microcontroller | ESP32 DevKit |
| Camera Module | ESP32-CAM (AI-Thinker) |
| RTC Module | DS3231 |
| Servo Motor | SG90 / MG995 |
| LCD Display | 16×2 with I2C (address `0x27`) |
| Buzzer | Active/Passive buzzer |
| LED | Any 5mm LED |
| Button | Tactile push button |
| FTDI Programmer | For flashing ESP32-CAM |
| Power Supply | 5V USB / Li-Po battery |

---

## 📐 Pin Configuration

### ESP32 DevKit (Main Controller)

```
ESP32 Pin   →   Component
─────────────────────────
GPIO 18     →   Servo Signal
GPIO 25     →   Buzzer
GPIO 26     →   LED
GPIO 4      →   Button (INPUT_PULLUP)
GPIO 21     →   I2C SDA (LCD + RTC)
GPIO 22     →   I2C SCL (LCD + RTC)
```

### ESP32-CAM (AI-Thinker)

```
ESP32-CAM Pin   →   Connection
───────────────────────────────
5V              →   5V Power
GND             →   GND
GPIO 0          →   GND (only during flashing, remove after)
U0TXD           →   FTDI RX (for flashing)
U0RXD           →   FTDI TX (for flashing)
```

> ⚠️ The ESP32-CAM has **no onboard USB**. You must use an FTDI USB-to-Serial adapter to flash it. Pull GPIO0 LOW during upload, then disconnect it to run.

---

## 📦 Libraries Required

### ESP32 DevKit
Install via Arduino IDE Library Manager:

- `BlynkSimpleEsp32` — Blynk IoT platform
- `RTClib` — DS3231 RTC
- `ESP32Servo` — Servo control for ESP32
- `LiquidCrystal_I2C` — I2C LCD driver
- `WiFi` *(built-in with ESP32 board package)*

### ESP32-CAM
Install via Arduino IDE Library Manager:

- `ESP32` board package by Espressif *(includes camera drivers)*
- `esp32cam` or use built-in `esp_camera.h` from the Espressif core

---

## ⚙️ Setup & Configuration

### 1. Clone the repo

```bash
git clone https://github.com/YOUR_USERNAME/CareNet.git
cd CareNet
```

### 2. Configure credentials

Open `firmware/CareNet_main/CareNet_main.ino` and update:

```cpp
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
```

Open `firmware/CareNet_cam/CareNet_cam.ino` and update:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Install ESP32 board in Arduino IDE

- Open **Preferences** → Additional Board URLs:
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- Install **esp32** by Espressif from Boards Manager.

### 4. Flash the Main Controller

- Select board: **ESP32 Dev Module**
- Upload `firmware/CareNet_main/CareNet_main.ino`

### 5. Flash the ESP32-CAM

- Select board: **AI Thinker ESP32-CAM**
- Connect FTDI adapter, pull **GPIO0 → GND**
- Upload `firmware/CareNet_cam/CareNet_cam.ino`
- After upload: **disconnect GPIO0 from GND**, press reset button

---

## 📷 ESP32-CAM — Live Monitoring

The ESP32-CAM connects to your WiFi and hosts a **live MJPEG video stream** accessible from any browser or the Blynk app on your local network.

```
http://<ESP32-CAM-IP-ADDRESS>/stream
```

**How to find the IP address:**
- Open Arduino IDE Serial Monitor at 115200 baud after booting
- The CAM will print its IP address on boot

**Use cases:**
- Caregiver can visually confirm the patient took the medicine
- Verify the dispenser mechanism is working correctly
- Remote monitoring of the dispenser unit

---

## 📱 Blynk Setup

1. Create a free account at [blynk.cloud](https://blynk.cloud)
2. Create a new Template:
   - Template ID: `TMPL3SXRXA7Xo`
   - Template Name: `smart medication 2`
3. Add a device and copy the **Auth Token**
4. Paste the token into the firmware
5. *(Optional)* Add a **Video Streaming** widget in Blynk and point it to `http://<ESP32-CAM-IP>/stream` for in-app live view

---

## 🔄 How It Works

```
System Boot
    │
    ├─→ [ESP32 DevKit] Connect WiFi + Sync NTP + Set RTC
    ├─→ [ESP32-CAM]    Connect WiFi + Start camera stream
    │
    └─→ Schedule first dose (now + 1 min)
         │
         ▼
    [Every loop — DevKit]
         │
         ├─ Time matches next dose?
         │       YES → Servo dispense + Buzzer alert + LCD update
         │
         ├─ Button pressed?
         │       YES → Manual dispense
         │
         └─ LCD updates every 500ms
                  └─ Shows: current time, last dose, next dose

    [ESP32-CAM — always running]
         └─ Serving live MJPEG stream at /stream
```

---

## 📁 Repository Structure

```
CareNet/
├── firmware/
│   ├── CareNet_main/
│   │   └── CareNet_main.ino    # ESP32 DevKit — main dispensing logic
│   └── CareNet_cam/
│       └── CareNet_cam.ino     # ESP32-CAM — live video stream server
├── docs/
│   ├── circuit_diagram.md      # Wiring guide (DevKit + CAM)
│   └── design_notes.md         # Architecture & design decisions
├── diagrams/
│   └── block_diagram.md        # System block diagram
├── blynk/
│   └── blynk_setup.md          # Blynk dashboard setup guide
├── .gitignore
└── README.md
```

---

## 🧠 Team CareNet

Built for **SYMBIOT Hackathon** — Problem Statement **SYMB0008**

> *"Because every dose matters."*

---

## 📄 License

MIT License — feel free to build upon this project!
