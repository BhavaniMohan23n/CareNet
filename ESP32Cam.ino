#include <dummy.h>

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMP------"
#define BLYNK_TEMPLATE_NAME "Smart Medication System"
#define BLYNK_AUTH_TOKEN "****************"
#define BlYNK_DEVICE_NAME"ESP32 Cam"
#include "esp_camera.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>


// ===== WIFI =====
char ssid[] = "wifi_name ";
char pass[] = "password";

// ===== PINS =====
#define IR_SENSOR_PIN 13
#define BUZZER_PIN 4
#define BUTTON_PIN 14

// ===== STATES =====
bool sent = false;
bool buzzerOn = false;
bool buzzerMuted = false;

// ===== BUZZER CONFIG (ESP32 CORE 3.x) =====
#define BUZZER_FREQ 2000
#define BUZZER_RES 8

WiFiServer server(80);

// ===== CAMERA PINS (AI THINKER) =====
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===== BUTTON CHECK =====
void checkButton() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    buzzerMuted = true;
  }
}

// ===== CAMERA INIT =====
void startCamera() {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    while (true);
  }

  Serial.println("Camera Ready");
}

// ===== HANDLE IR + BUZZER =====
void handleDetection() {
  int sensorValue = digitalRead(IR_SENSOR_PIN);

  if (sensorValue == LOW) {

    Blynk.virtualWrite(V0, 1);

    if (!buzzerOn && !buzzerMuted) {
      ledcWriteTone(BUZZER_PIN, BUZZER_FREQ);
      buzzerOn = true;
    }

    if (!sent) {
      Serial.println("Medicine Dispensed !");
      Blynk.logEvent("alert", "Medicine Dispensed !");
      sent = true;
    }

  } else {

    Blynk.virtualWrite(V0, 0);

    if (buzzerOn) {
      ledcWriteTone(BUZZER_PIN, 0);
      buzzerOn = false;
    }

    sent = false;
    buzzerMuted = false; // reset mute
  }

  // force OFF if muted
  if (buzzerMuted && buzzerOn) {
    ledcWriteTone(BUZZER_PIN, 0);
    buzzerOn = false;
  }
}

// ===== STREAM FUNCTION =====
void streamJPG(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
  client.println();

  while (client.connected()) {

    checkButton();
    handleDetection();

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) continue;

    client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);

    Blynk.run();
    delay(30);
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Buzzer setup
  ledcAttach(BUZZER_PIN, BUZZER_FREQ, BUZZER_RES);
  ledcWriteTone(BUZZER_PIN, 0);

  startCamera();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.print("Stream: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

// ===== LOOP =====
void loop() {
  Blynk.run();

  checkButton();
  handleDetection();

  WiFiClient client = server.available();

  if (client) {
    String req = client.readStringUntil('\r');

    if (req.indexOf("/stream") != -1) {
      streamJPG(client);
    } else {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<img src='/stream'>");
    }

    client.stop();
  }

  delay(50);
}
