#include "ble_lock.h"

class LockServerCallbacks : public BLEServerCallbacks {
protected:
    void onConnect(BLEServer* pServer) override {
        Serial.println("Client connected");
    }

    void onDisconnect(BLEServer* pServer) override {
        Serial.println("Client disconnected");
        // При разъединении, запускаем рекламу снова
        BLEDevice::startAdvertising();
    }
};

BLELock::BLELock(const std::string& name, const std::string& uuid) 
    : name_(name), uuid_(uuid), pServer_(nullptr), pService_(nullptr) {
    BLEDevice::init(name_);  // Инициализация BLE
    setupServer();  // Настройка BLE сервера
}

void BLELock::setupServer() {
    pServer_ = BLEDevice::createServer();  // Создание BLE сервера
    pServer_->setCallbacks(new LockServerCallbacks());  // Установка колбэков сервера
    setupService();  // Настройка BLE сервиса
}

void BLELock::setupService() {
    pService_ = pServer_->createService(uuid_);  // Создание BLE сервиса
}

void BLELock::addCharacteristic(const std::string& uuid, bool isWritable) {
    BLECharacteristic* pCharacteristic;
    
    if (isWritable) {
        pCharacteristic = pService_->createCharacteristic(
            uuid, 
            BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ
        );
    } else {
        pCharacteristic = pService_->createCharacteristic(
            uuid, 
            BLECharacteristic::PROPERTY_READ
        );
    }

    characteristics_.push_back(pCharacteristic);  // Добавление в список характеристик
}

void BLELock::addNotificationChannel(const std::string& uuid) {
    BLECharacteristic* pCharacteristic = pService_->createCharacteristic(
        uuid,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    BLE2902* pDescriptor = new BLE2902();  // Добавление дескриптора
    pDescriptor->setNotifications(true);  // Включить оповещения
    pCharacteristic->addDescriptor(pDescriptor);  // Привязка дескриптора к характеристике
}

void BLELock::begin() {
    pService_->start();  // Запуск сервиса
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(uuid_);  // Добавление UUID сервиса в рекламу
    BLEDevice::startAdvertising();  // Запуск рекламы
}

void BLELock::setCharacteristicCallback(const std::string& uuid, BLECharacteristicCallbacks* callback) {
    for (auto& characteristic : characteristics_) {
        if (characteristic->getUUID().toString() == uuid) {
            characteristic->setCallbacks(callback);  // Установка колбэка для характеристики
            break;
        }
    }
}

BLECharacteristic* BLELock::getCharacteristic(const std::string& uuid) {
    for (auto characteristic : characteristics_) {
        if (characteristic->getUUID().toString() == uuid) {
            return characteristic;  // Возвращение соответствующей характеристики
        }
    }
    return nullptr;  // Если не найдено, возвращаем nullptr
}
