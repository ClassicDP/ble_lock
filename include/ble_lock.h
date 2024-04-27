#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <vector>

class BLELock
{
public:
    BLELock(const std::string &name, const std::string &uuid);                                     // Инициализация по имени и UUID
    void addCharacteristic(const std::string &uuid, bool isWritable = false);                      // Добавление характеристики
    void addNotificationChannel(const std::string &uuid);                                          // Добавление канала оповещения
    void begin();                                                                                  // Запуск BLE
    void setCharacteristicCallback(const std::string &uuid, BLECharacteristicCallbacks *callback); // Установка колбэков для характеристик
    BLECharacteristic *getCharacteristic(const std::string &uuid);

private:
    std::string name_;                                 // Имя BLE устройства
    std::string uuid_;                                 // UUID BLE сервиса
    BLEServer *pServer_;                               // Указатель на BLE сервер
    BLEService *pService_;                             // Указатель на BLE сервис
    std::vector<BLECharacteristic *> characteristics_; // Список характеристик

    void setupServer();  // Инициализация сервера
    void setupService(); // Инициализация сервиса
};
