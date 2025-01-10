#ifndef CONFIG_H
#define CONFIG_H

// Параметры сканирования BLE
#define SCAN_TIME 5 // Длительность сканирования в секундах
#define SCAN_INTERVAL 10000 // Интервал сканирования BLE в миллисекундах (10 секунд)

// Параметры игры
#define COIN_UPDATE_INTERVAL 1000 // Интервал обновления монет в миллисекундах (1 секунда)
#define SAVE_PROGRESS_INTERVAL 10 // Сохранение прогресса каждые 10 секунд

// Параметры дисплея
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

// Настройки SD-карты
#define SD_CS 5 // Контакт выбора чипа для SD-карты

// Информация о разработчике
#define GAME_VERSION "v1.1"
#define DEVELOPER_NAME "houneTeam"

#endif // CONFIG_H
