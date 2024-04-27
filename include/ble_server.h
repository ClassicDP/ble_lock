#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <unordered_map>
#include <ctime>
#include "vector"

#define SERVUUID "6e598b6e-de9f-44a8-9940-591509144b8a" // UUID сервиса замка
#define MAC_CHARACTERISTIC "6261953f-dcdb-4d7b-841c-414d32dea3d7"                // Характеристика для передачи MAC-адреса
#define ACTUALPERIOD 1000

struct KeyInfo
{
    std::string macAddress;
    time_t lastConnectionTime; // Время последнего подключения
};

class BLELockServer
{
public:
    BLELockServer(const std::string &deviceName)
        : deviceName(deviceName)
    {
        BLEDevice::init(deviceName);
        pServer = BLEDevice::createServer();
        pServer->setCallbacks(new ServerCallbacks(this));
        createService();
        startAdvertising();

    }

    void startAdvertising()
    {
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVUUID);
        pAdvertising->setScanResponse(true);
         pAdvertising->setMinPreferred(0x0);
        BLEDevice::startAdvertising();
        Serial.println("Advertising started");
    }

    void createService()
    {
        pService = pServer->createService(SERVUUID);

        // Характеристика для приема MAC-адресов
        pCharacteristic = pService->createCharacteristic(
            MAC_CHARACTERISTIC,
            BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY // Разрешаем запись и уведомления
        );

        pCharacteristic->setCallbacks(new CharacteristicCallbacks(this));
        pService->start();
    }

    void addOrUpdateKey(const std::string &macAddress)
    {
        time_t now = std::time(nullptr);               // Получаем текущее время
        connectedKeys[macAddress] = {macAddress, now}; // Добавляем или обновляем информацию о ключе
    }

    std::vector<std::string> getConnectedKeys()
    {
        std::vector<std::string> v;
        auto t = std::time(nullptr);
        for (auto &key : connectedKeys)
        {
            if ((t - key.second.lastConnectionTime) < ACTUALPERIOD)
                v.push_back(key.first);
        }
        return v; // Возвращаем список подключенных ключей
    }

private:
    class CharacteristicCallbacks : public BLECharacteristicCallbacks
    {
    public:
        CharacteristicCallbacks(BLELockServer *server) : server(server) {}

        void onWrite(BLECharacteristic *pCharacteristic) override
        {
            std::string macAddress = pCharacteristic->getValue(); // Получаем MAC-адрес
            server->addOrUpdateKey(macAddress);                   // Добавляем или обновляем ключ в списке
            Serial.print("MAC Address received: ");
            Serial.println(macAddress.c_str());
        }

    private:
        BLELockServer *server;
    };

    class ServerCallbacks : public BLEServerCallbacks
    {
    public:
        ServerCallbacks(BLELockServer *server) : server(server) {}

        void onConnect(BLEServer *pServer) override
        {
            Serial.println("Device connected");
        }

        void onDisconnect(BLEServer *pServer) override
        {
            server->startAdvertising(); // Перезапускаем рекламу при отключении
            Serial.println("Device disconnected");
        }

    private:
        BLELockServer *server;
    };

    std::unordered_map<std::string, KeyInfo> connectedKeys; // Список подключенных ключей
    std::string deviceName;
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pCharacteristic;
};
