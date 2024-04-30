#include "ble_lock.h"

class LockServerCallbacks : public BLEServerCallbacks
{
protected:
    void onConnect(BLEServer *pServer) override
    {
        Serial.println("Client connected");
    }

    void onDisconnect(BLEServer *pServer) override
    {
        Serial.println("Client disconnected");
        // При разъединении, запускаем рекламу снова
        BLEDevice::startAdvertising();
    }
};

// Конструктор класса BLELock
BLELock::BLELock(const std::string& name, const std::string& uuid)
    : name_(name), uuid_(uuid), pServer_(nullptr), pService_(nullptr) {
    BLEDevice::init(name_);  // Инициализация BLE
    setupServer();  // Настройка BLE-сервера
}

// Настройка BLE-сервера
void BLELock::setupServer() {
    pServer_ = BLEDevice::createServer();  // Создание BLE-сервера
    pServer_->setCallbacks(new LockServerCallbacks());  // Установка колбэков сервера
    setupService();  // Настройка BLE-сервиса
}

// Настройка BLE-сервиса
void BLELock::setupService() {
    pService_ = pServer_->createService(uuid_);  // Создание BLE-сервиса
}

// Добавление характеристики
void BLELock::addCharacteristic(const std::string& uuid, bool isWritable) {
    BLECharacteristic* pCharacteristic;

    if (isWritable) {
        pCharacteristic = pService_->createCharacteristic(
            uuid,
            BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
    } else {
        pCharacteristic = pService_->createCharacteristic(
            uuid,
            BLECharacteristic::PROPERTY_READ);
    }

    characteristicMap_[uuid] = pCharacteristic;  // Добавление характеристики в map
}

// Добавление канала оповещения
void BLELock::addNotificationChannel(const std::string& uuid) {
    BLECharacteristic* pCharacteristic = pService_->createCharacteristic(
        uuid,
        BLECharacteristic::PROPERTY_NOTIFY);

    BLE2902* pDescriptor = new BLE2902();  // Создание дескриптора
    pDescriptor->setNotifications(true);  // Включить оповещения
    pCharacteristic->addDescriptor(pDescriptor);  // Добавление дескриптора к характеристике

    characteristicMap_[uuid] = pCharacteristic;  // Добавление в map
}

// Запуск BLE-сервиса
void BLELock::begin() {
    pService_->start();  // Запуск сервиса
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();  // Получение рекламы
    pAdvertising->addServiceUUID(uuid_);  // Добавление UUID сервиса в рекламу
    BLEDevice::startAdvertising();  // Запуск рекламы
}

// Установка колбэков для характеристики по UUID
void BLELock::setCharacteristicCallback(const std::string& uuid, BLECharacteristicCallbacks* callback) {
    auto it = characteristicMap_.find(uuid);  // Поиск характеристики в map
    if (it != characteristicMap_.end()) {  // Если найдена
        it->second->setCallbacks(callback);  // Установка колбэка
    }
}

// Получение характеристики по UUID
BLECharacteristic* BLELock::getCharacteristic(const std::string& uuid) {
    auto it = characteristicMap_.find(uuid);  // Поиск характеристики в map
    if (it != characteristicMap_.end()) {  // Если найдена
        return it->second;  // Возвращение характеристики
    }
    return nullptr;  // Если не найдено, возвращаем nullptr
}