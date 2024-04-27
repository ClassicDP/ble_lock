#include "WiFiManager.h"

WiFiManager::WiFiManager() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
  }
}

bool WiFiManager::connectToSavedNetwork() {
  WiFiConfig config = loadWiFiConfig();
  if (config.ssid.length() > 0) {
    WiFi.begin(config.ssid.c_str(), config.password.c_str());
    int maxAttempts = 10;
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < maxAttempts) {
      delay(1000);
      attempt++;
    }
    return WiFi.status() == WL_CONNECTED;
  }
  return false;
}

bool WiFiManager::connectToNetwork(const String &ssid, const String &password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  int maxAttempts = 10;
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < maxAttempts) {
    delay(1000);
    attempt++;
  }
  return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::saveWiFiConfig(const String &ssid, const String &password) {
  File file = SPIFFS.open("/wifi_config.json", "w");
  if (file) {
    DynamicJsonDocument doc(128);
    doc["ssid"] = ssid;
    doc["password"] = password;
    serializeJson(doc, file);
    file.close();
  }
}

WiFiConfig WiFiManager::loadWiFiConfig() {
  WiFiConfig config;
  File file = SPIFFS.open("/wifi_config.json", "r");
  if (file) {
    DynamicJsonDocument doc(128);
    if (!deserializeJson(doc, file)) {
      config.ssid = doc["ssid"].as<String>();
      config.password = doc["password"].as<String>();
    }
    file.close();
  }
  return config;
}

void WiFiManager::setupAccessPoint(const char *ssid, const char *password) {
  WiFi.softAP(ssid, password);
}
