#ifndef HTTP_ROUTER_H
#define HTTP_ROUTER_H

#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

String getContentType(const String &filename); // Объявление функции
String jsonToString(DynamicJsonDocument &json); // Объявление функции

class HTTPRouter {
public:
  HTTPRouter(WiFiManager &wiFiManager);
  void start();
  void handleClient();
  void handleFile();
  void handleNetworks();
  void handleConnect();

  private:
  WebServer server;
  WiFiManager &wiFiManager;
};

#endif // HTTP_ROUTER_H
