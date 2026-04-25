# CareNet — Circuit Wiring Guide

## ESP32 Pin Mapping

| ESP32 GPIO | Component       | Connection Point     |
|------------|-----------------|----------------------|
| GPIO 18    | Servo Motor     | Signal wire          |
| GPIO 25    | Buzzer          | Positive terminal    |
| GPIO 26    | LED             | Anode (+ via 220Ω)   |
| GPIO 4     | Push Button     | One leg (other → GND)|
| GPIO 21    | SDA             | LCD SDA + RTC SDA    |
| GPIO 22    | SCL             | LCD SCL + RTC SCL    |
| 3.3V       | RTC VCC         | DS3231 VCC           |
| 5V         | LCD VCC         | LCD I2C module VCC   |
| 5V         | Servo VCC       | Servo red wire       |
| GND        | All GND         | Common ground        |

---

## I2C Bus

Both the **LCD (0x27)** and **DS3231 RTC** share the I2C bus:

```
ESP32 GPIO21 (SDA) ─┬─ LCD SDA
                    └─ RTC SDA

ESP32 GPIO22 (SCL) ─┬─ LCD SCL
                    └─ RTC SCL
```

Add 4.7kΩ pull-up resistors on SDA and SCL to 3.3V if you experience I2C instability.

---

## Servo Motor

```
Servo Red    → 5V
Servo Brown  → GND
Servo Orange → GPIO 18
```

> ⚠️ Power the servo from external 5V if it causes brownout on USB power.

---

## Button Wiring (INPUT_PULLUP)

```
Button Pin 1 → GPIO 4
Button Pin 2 → GND
```
Internal pull-up is enabled in firmware; no external resistor needed.

---

## LED Wiring

```
GPIO 26 → 220Ω resistor → LED Anode (+)
LED Cathode (−) → GND
```
