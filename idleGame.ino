#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include "config.h" // Подключение файла конфигурации

// Настройки дисплея и сенсорного экрана
TFT_eSPI tft = TFT_eSPI();
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// Параметры BLE
BLEScan *pBLEScan;

// Параметры игры
unsigned long bleCoins = 0; // Количество монет BLE
unsigned long deviceCount = 0; // Количество найденных устройств BLE
unsigned long lastCoinUpdate = 0; // Время последнего обновления монет
unsigned long lastDeviceScan = 0; // Время последнего сканирования BLE

// Объявление функций
void displayGameScreen();
void loadProgress();
void saveProgress();
void scanBLEDevices();

void setup() {
    Serial.begin(115200);

    // Инициализация сенсорного экрана
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(1);

    // Инициализация дисплея
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // Инициализация BLE
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);

    // Инициализация SD-карты
    if (!SD.begin(SD_CS)) {
        Serial.println("Ошибка инициализации SD-карты!");
    }

    // Загрузка прогресса из файла
    loadProgress();

    // Отображение начального экрана
    displayGameScreen();
}

void loop() {
    unsigned long currentMillis = millis();

    // Обновление монет BLE каждую секунду
    if (currentMillis - lastCoinUpdate >= COIN_UPDATE_INTERVAL) {
        lastCoinUpdate = currentMillis;
        bleCoins += deviceCount;

        // Сохранение прогресса каждые SAVE_PROGRESS_INTERVAL секунд
        static int saveCounter = 0;
        if (++saveCounter >= SAVE_PROGRESS_INTERVAL) {
            saveCounter = 0;
            saveProgress();
        }

        // Обновление отображения монет BLE
        tft.fillRect(0, 50, DISPLAY_WIDTH, 40, TFT_BLACK);
        tft.setCursor(10, 50);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.printf("BLECoins: %lu", bleCoins);
    }

    // Сканирование BLE-устройств каждые 10 секунд
    if (currentMillis - lastDeviceScan >= SCAN_INTERVAL) {
        lastDeviceScan = currentMillis;
        scanBLEDevices();

        // Обновление отображения количества устройств BLE
        tft.fillRect(0, 30, DISPLAY_WIDTH, 20, TFT_BLACK);
        tft.setCursor(10, 30);
        tft.setTextSize(2);
        tft.printf("BLESpeed: %lu ble/s", deviceCount);
    }
}

// Функция для сканирования BLE-устройств
void scanBLEDevices() {
    BLEScanResults foundDevices = *pBLEScan->start(SCAN_TIME, false);
    deviceCount = foundDevices.getCount();
    pBLEScan->clearResults();

    Serial.printf("Найдено устройств: %lu\n", deviceCount);
}

// Функция для отображения начального экрана
void displayGameScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);

    tft.setCursor(10, 30);
    tft.printf("BLESpeed: %lu ble/s", deviceCount);

    tft.setCursor(10, 50);
    tft.printf("BLECoins: %lu", bleCoins);

    // Отображение версии игры и имени разработчика
    tft.setTextSize(1);
    tft.setCursor(DISPLAY_WIDTH - 100, DISPLAY_HEIGHT - 10);
    tft.printf("%s - %s", GAME_VERSION, DEVELOPER_NAME);
}

// Функция для загрузки прогресса из файла
void loadProgress() {
    if (SD.exists("/progress.json")) {
        File progressFile = SD.open("/progress.json", FILE_READ);
        if (progressFile) {
            StaticJsonDocument<500> doc;
            DeserializationError error = deserializeJson(doc, progressFile);
            if (!error) {
                bleCoins = doc["bleCoins"] | 0;
                deviceCount = doc["deviceCount"] | 0;
            } else {
                Serial.println("Ошибка при чтении JSON-файла");
            }
            progressFile.close();
        } else {
            Serial.println("Не удалось открыть файл прогресса");
        }
    }
}

// Функция для сохранения прогресса в файл
void saveProgress() {
    File progressFile = SD.open("/progress.json", FILE_WRITE);
    if (progressFile) {
        StaticJsonDocument<500> doc;
        doc["bleCoins"] = bleCoins;
        doc["deviceCount"] = deviceCount;
        if (serializeJson(doc, progressFile) == 0) {
            Serial.println("Ошибка записи в файл");
        }
        progressFile.close();
    } else {
        Serial.println("Не удалось открыть файл для записи");
    }
}



