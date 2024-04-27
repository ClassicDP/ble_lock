#include "HTTPRouter.h"
#include "WiFiManager.h"

String getContentType(const String &filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    if (filename.endsWith(".css"))
        return "text/css";
    if (filename.endsWith(".js"))
        return "application/javascript";
    return "application/octet-stream";
}

String jsonToString(DynamicJsonDocument &json)
{
    String s;
    serializeJson(json, s);
    return s;
}

HTTPRouter::HTTPRouter(WiFiManager &wiFiManager) : server(80), wiFiManager(wiFiManager) {}

void HTTPRouter::start()
{
    server.on("/", [this]()
              { handleFile(); });
    server.on("/networks", [this]()
              { handleNetworks(); });
    server.on("/connect", HTTP_POST, [this]()
              { handleConnect(); });

    server.onNotFound([this]()
                      { handleFile(); });

    server.begin();
}

void HTTPRouter::handleClient()
{
    server.handleClient();
}

void HTTPRouter::handleFile()
{
    String path = server.uri();
    if (path == "/")
    {
        path = "/index.html";
    }

    if (SPIFFS.exists(path))
    {
        File file = SPIFFS.open(path, "r");
        server.streamFile(file, getContentType(path));
        file.close();
    }
    else
    {
        server.send(404, "File not found");
    }
}

void HTTPRouter::handleNetworks()
{
    int networksFound = WiFi.scanNetworks();
    DynamicJsonDocument doc(512);
    JsonArray networks = doc.createNestedArray("networks");

    for (int i = 0; i < networksFound; i++)
    {
        JsonObject network = networks.createNestedObject();
        network["ssid"] = WiFi.SSID(i);
        network["rssi"] = WiFi.RSSI(i);
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
}

void HTTPRouter::handleConnect()
{
    Serial.println("Trying to connect...");
    String network = server.arg("network");
    String password = server.arg("password");
    Serial.println("SID: " + network + "pass: " + password);

    if (this->wiFiManager.connectToNetwork(network, password))
    {
        this->wiFiManager.saveWiFiConfig(network, password);
        DynamicJsonDocument doc(256);
        doc["status"] = "success";
        doc["ip"] = WiFi.localIP().toString();
        server.send(200, "application/json", jsonToString(doc));
    }
    else
    {
        DynamicJsonDocument doc(256);
        doc["status"] = "failure";
        server.send(200, "application/json", jsonToString(doc));
    }
}
