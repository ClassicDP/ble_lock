#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

struct WiFiConfig {
  String ssid;
  String password;
};

class WiFiManager {
public:
  WiFiManager();
  
  bool connectToSavedNetwork();
  bool connectToNetwork(const String &ssid, const String &password);
  void saveWiFiConfig(const String &ssid, const String &password);
  WiFiConfig loadWiFiConfig();
  void setupAccessPoint(const char *ssid, const char *password);
  
private:
  WiFiConfig currentConfig;
};

#endif
