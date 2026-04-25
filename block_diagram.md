# CareNet — System Block Diagram

```
                        ┌─────────────────────────────────┐
                        │          ESP32 DevKit            │
                        │                                  │
  ┌──────────┐  I2C    │  GPIO21 (SDA) ── GPIO22 (SCL)   │
  │ DS3231   │─────────│                                  │
  │  RTC     │         │  GPIO18 ── Servo Signal          │──── Servo Motor
  └──────────┘         │  GPIO25 ── Buzzer                │──── Buzzer
                        │  GPIO26 ── LED                   │──── LED
  ┌──────────┐  I2C    │  GPIO4  ── Button                │──── Push Button
  │  16×2    │─────────│                                  │
  │   LCD    │         │  WiFi ──────────────────────────────── Internet
  └──────────┘         │                                  │
                        └─────────────────────────────────┘
                                        │
                                   Blynk IoT
                                        │
                               ┌────────────────┐
                               │  Mobile App    │
                               │  Dashboard     │
                               └────────────────┘
```

## Data Flow

```
NTP Server
    │  (time sync on boot)
    ▼
DS3231 RTC ──→ ESP32 reads time every loop
                    │
                    ├──→ LCD: Shows HH:MM:SS + dose status
                    │
                    ├──→ Dose time match?
                    │         YES ──→ Servo dispense + Buzzer alert
                    │
                    └──→ Button pressed?
                              YES ──→ Manual dispense
```
