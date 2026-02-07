/*
  ESP32 Bitcoin Price Tracker with TFT Display

  Author: Shrey Verma
  Description:
  Fetches live Bitcoin price from a public API and displays it on a
  1.8" SPI TFT display using an ESP32 DevKit V1.

  Notes:
  - TFT RESET and LED pins are tied to 3.3V
  - Uses HTTPS with certificate verification disabled
*/


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <Fonts/Free_Fonts.h>
#include <time.h>

// ====== WiFi CREDENTIALS ======
const char* ssid = ""YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ====== API URL ======
const char* apiUrl = "INSERT_YOUR_API_ENDPOINT_URL";

// ====== TFT Display ======
TFT_eSPI tft = TFT_eSPI();

// ====== FUNCTION TO SHOW WIFI ERROR ======
void showWiFiError() {
  tft.fillScreen(TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setTextSize(3);
  tft.setCursor(10, 40);
  tft.println("WiFi Error!");
}

// ====== SETUP ======
void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Initial message
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Connecting WiFi...");

  // Keep trying until WiFi connects
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.setCursor(10, 40);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("Trying...");
  }

  // Connection successful message 
  Serial.println("\nWiFi connected");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi connected");

  // Sync time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

// ====== LOOP ======
void loop() {
  // If WiFi disconnects, show error & retry
  if (WiFi.status() != WL_CONNECTED) {
    showWiFiError();
    WiFi.reconnect();
    delay(5000);
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();  // bypass TLS certificate check

  HTTPClient https;
  https.begin(client, apiUrl);
  int httpCode = https.GET();

  if (httpCode == 200) {
    String payload = https.getString();
    Serial.println(payload);

    // Extract clean JSON
    int jsonStart = payload.indexOf('{');
    int jsonEnd = payload.lastIndexOf('}');
    String jsonStr = payload.substring(jsonStart, jsonEnd + 1);

    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, jsonStr) == DeserializationError::Ok) {
      float price = doc["bitcoin"]["usd"];

      // Draw to TFT
      tft.fillScreen(TFT_BLACK);

      // Title
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      tft.setTextSize(2);
      tft.setCursor(10, 10);
      tft.println("Bitcoin Price");

      // Display Price (CUSTOMIZABLE FONT SIZE & COLOR)
      tft.setTextColor(TFT_GREEN, TFT_BLACK);   // TEXT_COLOR, BACKGROUND_COLOR
      tft.setTextSize(2);                       // 1=small … 4=large
      tft.setCursor(10, 50);
      tft.print("$");
      tft.println(price);

      // Time
      time_t now = time(nullptr);
      struct tm* timeinfo = localtime(&now);
      char buf[32];
      strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);

      tft.setTextColor(TFT_CYAN, TFT_BLACK); // date
      tft.setTextSize(1.5);
      tft.setCursor(10, 110);
      tft.println(buf);
    }
  } else {
    // Show HTTP error on TFT
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setTextSize(2);
    tft.setCursor(10, 40);
    tft.println("HTTP Error");
  }

  https.end();
  delay(60000); // refreshes every 1 minute
}