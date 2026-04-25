# 💊 CareNet — Smart Medication Reminder & Automated Dispensing System

> **Hackathon:** SYMBIOT | **Problem Statement:** SYMB0008  
> **Domain:** Embedded Systems & IoT | **Team:** CareNet

---

## 📌 Problem Statement

Medication adherence is a major challenge, especially for elderly individuals and patients with chronic illnesses. Missing doses or incorrect timing can lead to serious health complications.

CareNet is an intelligent embedded system that reminds users to take medication on time and automatically dispenses the correct dosage — with alerts, schedule tracking, and reliable real-world operation.

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

---

## 🛠️ Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 DevKit |
| RTC Module | DS3231 |
| Servo Motor | SG90 / MG995 |
| LCD Display | 16×2 with I2C (address `0x27`) |
| Buzzer | Active/Passive buzzer |
| LED | Any 5mm LED |
| Button | Tactile push button |
| Power Supply | 5V USB / Li-Po battery |

---

## 📐 Pin Configuration

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

---

## 📦 Libraries Required

Install these via Arduino IDE Library Manager:

- `BlynkSimpleEsp32` — Blynk IoT platform
- `RTClib` — DS3231 RTC
- `ESP32Servo` — Servo control for ESP32
- `LiquidCrystal_I2C` — I2C LCD driver
- `WiFi` *(built-in with ESP32 board package)*

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

### 3. Install ESP32 board in Arduino IDE

- Open **Preferences** → Additional Board URLs:
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- Install **esp32** by Espressif from Boards Manager.

### 4. Flash the firmware

- Select board: **ESP32 Dev Module**
- Upload `firmware/CareNet_main/CareNet_main.ino`

---

## 📱 Blynk Setup

1. Create a free account at [blynk.cloud](https://blynk.cloud)
2. Create a new Template:
   - Template ID: `TMPL3SXRXA7Xo`
   - Template Name: `smart medication 2`
3. Add a device and copy the **Auth Token**
4. Paste the token into the firmware

---

## 🔄 How It Works

```
System Boot
    │
    ├─→ Connect WiFi
    ├─→ Sync time via NTP (IST)
    ├─→ Set RTC
    └─→ Schedule first dose (now + 1 min)
         │
         ▼
    [Every loop]
         │
         ├─ Time matches next dose?
         │       YES → Dispense + Buzzer + Update LCD
         │
         ├─ Button pressed?
         │       YES → Manual dispense
         │
         └─ LCD updates every 500ms
                  └─ Shows: current time, last dose, next dose
```

---

## 📁 Repository Structure

```
CareNet/
├── firmware/
│   └── CareNet_main/
│       └── CareNet_main.ino    # Main ESP32 firmware
├── docs/
│   ├── circuit_diagram.md      # Wiring guide
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
