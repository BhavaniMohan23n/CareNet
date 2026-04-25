/*
 * CareNet — ESP32-CAM Live Stream Server
 * Board: AI Thinker ESP32-CAM
 *
 * Hosts a live MJPEG camera stream on your local WiFi network.
 * Access stream at: http://<THIS_DEVICE_IP>/stream
 *
 * Flash instructions:
 *  1. Connect FTDI adapter to ESP32-CAM (TX→RX, RX→TX, GND→GND, 5V→5V)
 *  2. Pull GPIO0 → GND before powering on (enables flash mode)
 *  3. Upload this sketch with board set to "AI Thinker ESP32-CAM"
 *  4. After upload: disconnect GPIO0 from GND, press reset
 */

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

/******** WIFI CREDENTIALS ********/
const char* ssid     = "YOUR_WIFI_SSID";      // Change this
const char* password = "YOUR_WIFI_PASSWORD";   // Change this

/******** AI-THINKER ESP32-CAM PIN MAP ********/
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

/******** MJPEG STREAM BOUNDARY ********/
#define PART_BOUNDARY "123456789000000000000987654321"

static const char* STREAM_CONTENT_TYPE =
  "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* STREAM_PART =
  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;

/******** STREAM HANDLER ********/
static esp_err_t stream_handler(httpd_req_t* req) {
  camera_fb_t* fb = NULL;
  esp_err_t res   = ESP_OK;
  char part_buf[64];

  res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("[CareNet-CAM] Camera capture failed");
      res = ESP_FAIL;
    } else {
      // Send boundary
      res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));

      if (res == ESP_OK) {
        size_t hlen = snprintf(part_buf, sizeof(part_buf), STREAM_PART, fb->len);
        res = httpd_resp_send_chunk(req, part_buf, hlen);
      }

      if (res == ESP_OK) {
        res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
      }

      esp_camera_fb_return(fb);
    }

    if (res != ESP_OK) break;
  }

  return res;
}

/******** START STREAM SERVER ********/
void startStreamServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port    = 80;

  httpd_uri_t stream_uri = {
    .uri      = "/stream",
    .method   = HTTP_GET,
    .handler  = stream_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    Serial.println("[CareNet-CAM] Stream server started.");
  }
}

/******** SETUP ********/
void setup() {
  Serial.begin(115200);
  Serial.println("[CareNet-CAM] Booting...");

  // Camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_VGA;   // 640x480 — good balance
  config.jpeg_quality = 12;              // 0–63, lower = better quality
  config.fb_count     = 1;

  // Init camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[CareNet-CAM] Camera init FAILED: 0x%x\n", err);
    return;
  }
  Serial.println("[CareNet-CAM] Camera initialized.");

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("[CareNet-CAM] Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("[CareNet-CAM] Connected! Stream at: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream");

  startStreamServer();
}

/******** LOOP ********/
void loop() {
  // Stream server runs on its own task — nothing needed here
  delay(10000);
}
