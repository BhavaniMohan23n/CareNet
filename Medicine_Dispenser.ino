#define BLYNK_TEMPLATE_ID "TMPL3SXRXA7Xo"
#define BLYNK_TEMPLATE_NAME "smart medication 2"
#define BLYNK_AUTH_TOKEN "XXXXXXX"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <RTClib.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

/******** WIFI ********/
char ssid[] = "wiFi name";
char pass[] = "password";

/******** OBJECTS ********/
RTC_DS3231 rtc;
Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

/******** PINS ********/
#define SERVO_PIN 18
#define BUZZER 25
#define LED 26
#define BUTTON 4

/******** VARIABLES ********/
int nextHour;
int nextMinute;

int lastHour = -1;
int lastMinute = -1;

bool triggered = false;
bool alarmActive = false;

unsigned long lastLCD = 0;
unsigned long lastBlink = 0;
bool ledState = false;

/******** SERVO (FORWARD ONLY) ********/
void dispenseMedicine() {

  Serial.println("SERVO FORWARD ONLY");

  digitalWrite(LED, HIGH);   // LED ON during dispensing

  servo.write(90);           // ✔ MOVE FORWARD ONLY
  delay(1200);               // hold position

  // ❌ NO servo.write(0)
  // ❌ NO backward movement

  digitalWrite(LED, LOW);    // LED OFF after dispensing
}

/******** NEXT DOSE ********/
void moveNextDoseForward() {

  nextMinute += 1;

  if (nextMinute >= 60) {
    nextMinute = 0;
    nextHour = (nextHour + 1) % 24;
  }
}

/******** SETUP ********/
void setup() {

  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  rtc.begin();

  servo.attach(SERVO_PIN);
  servo.write(0);   // start position only ONCE

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  configTime(19800, 0, "pool.ntp.org");

  struct tm t;
  while (!getLocalTime(&t)) delay(500);

  rtc.adjust(DateTime(
    t.tm_year + 1900,
    t.tm_mon + 1,
    t.tm_mday,
    t.tm_hour,
    t.tm_min,
    t.tm_sec
  ));

  DateTime now = rtc.now();

  nextHour = now.hour();
  nextMinute = now.minute() + 1;

  lcd.clear();
  lcd.print("SYSTEM READY");
  delay(1500);
  lcd.clear();
}

/******** LOOP ********/
void loop() {

  DateTime now = rtc.now();

  int h = now.hour();
  int m = now.minute();
  int s = now.second();

  /******** TRIGGER ********/
  if (!triggered &&
      h == nextHour &&
      m == nextMinute) {

    alarmActive = true;
    triggered = true;

    lastHour = nextHour;
    lastMinute = nextMinute;

    dispenseMedicine();
    tone(BUZZER, 2000);

    moveNextDoseForward();
  }

  /******** RESET ********/
  if (h != nextHour || m != nextMinute) {
    alarmActive = false;
    triggered = false;
    noTone(BUZZER);
  }

  /******** LCD ********/
  if (millis() - lastLCD > 500) {
    lastLCD = millis();

    lcd.setCursor(0, 0);
    lcd.print("T ");
    if (h < 10) lcd.print("0"); lcd.print(h);
    lcd.print(":");
    if (m < 10) lcd.print("0"); lcd.print(m);
    lcd.print(":");
    if (s < 10) lcd.print("0"); lcd.print(s);

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);

    if (alarmActive) {
      lcd.print("TAKE MEDICINE");
    }
    else if (lastHour != -1) {

      lcd.print("L:");
      if (lastHour < 10) lcd.print("0");
      lcd.print(lastHour);
      lcd.print(":");
      if (lastMinute < 10) lcd.print("0");
      lcd.print(lastMinute);

      lcd.print(" N:");
      if (nextHour < 10) lcd.print("0");
      lcd.print(nextHour);
      lcd.print(":");
      if (nextMinute < 10) lcd.print("0");
      lcd.print(nextMinute);
    }
    else {

      lcd.print("Next ");
      if (nextHour < 10) lcd.print("0");
      lcd.print(nextHour);
      lcd.print(":");
      if (nextMinute < 10) lcd.print("0");
      lcd.print(nextMinute);
    }
  }

  /******** BUTTON ********/
  if (digitalRead(BUTTON) == LOW) {
    delay(50);

    if (digitalRead(BUTTON) == LOW) {

      alarmActive = false;
      triggered = true;

      noTone(BUZZER);

      dispenseMedicine();

      while (digitalRead(BUTTON) == LOW);
    }
  }
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
