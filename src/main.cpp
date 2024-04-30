#include <Arduino.h>
#include "WiFiManager.h"
#include "HTTPRouter.h"
#include "ble_lock.h"

#define LOCK_SERVICE_UUID "550e8400-e29b-41d4-a716-446655440000"
#define LOCK_STATUS_UUID "e7e478d3-8d98-40c8-8f8f-d1c94e441b21"
#define LOCK_CONTROL_UUID "1b3e8c6a-f7d5-49c9-b2a0-50e5722dbb6f"
#define LOCK_NOTIFICATION_UUID "3c29d97c-92a1-4a64-bc7c-f58bcaaf9af4"


#define AP_SSID "MyESP32AP"
#define AP_PASSWORD "12345678"
#define NOTIFICATION_INTERVAL 100  // Интервал оповещений в миллисекундах

WiFiManager wifiManager;
HTTPRouter httpRouter(wifiManager);

BLELock* myLock;

// Указатель на канал оповещения
BLECharacteristic* notificationCharacteristic;

// Класс колбэков для характеристики
class LockCharacteristicCallbacks : public BLECharacteristicCallbacks {
protected:
    void onWrite(BLECharacteristic* pCharacteristic) override {
        std::string value = pCharacteristic->getValue();
        Serial.println(("Characteristic written: " + value).c_str());
    }
};



unsigned long lastNotificationTime = 0;  // Последнее время оповещения

void setup() {
    Serial.begin(115200);

    // Подключение к сохраненной сети WiFi
    if (!wifiManager.connectToSavedNetwork()) {
        // Если не удалось подключиться, создается точка доступа
        wifiManager.setupAccessPoint(AP_SSID, AP_PASSWORD);
    }

    // Запуск HTTP сервера
    httpRouter.start();

    // Создание объекта BLE замка
    myLock = new BLELock("MyBLELock", LOCK_SERVICE_UUID);

    // Добавление характеристик
    myLock->addCharacteristic(LOCK_STATUS_UUID, false);  // Только для чтения
    myLock->addCharacteristic(LOCK_CONTROL_UUID, true);  // Для записи
    myLock->addNotificationChannel(LOCK_NOTIFICATION_UUID);  // Канал оповещения

    // Запуск BLE сервиса и рекламы
    myLock->begin();

    // Установка колбэка для управляющей характеристики
    myLock->setCharacteristicCallback(LOCK_CONTROL_UUID, new LockCharacteristicCallbacks());

    // Получаем указатель на канал оповещения
    notificationCharacteristic = myLock->getCharacteristic(LOCK_NOTIFICATION_UUID);

    // Инициализируем время последнего оповещения
    lastNotificationTime = millis();
}

void loop() {
    // Обработка входящих HTTP запросов
    httpRouter.handleClient();

    // Проверка, прошел ли заданный интервал для оповещения
    if (millis() - lastNotificationTime >= NOTIFICATION_INTERVAL) {
        if (notificationCharacteristic) {
            // Установка значения для оповещения
            notificationCharacteristic->setValue(String(millis()).c_str());
            // Отправка уведомления
            notificationCharacteristic->notify();

            // Обновление времени последнего оповещения
            lastNotificationTime = millis();
        }
    }
}