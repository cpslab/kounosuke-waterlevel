#include <Arduino.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "lib.hpp"
#include "modem.hpp"
#include "serial_protocol.hpp"

extern HardwareSerial MySerial0;
extern int NORMALTIMEOUT;
extern int POSTTIMEOUT;

void esp32c3_deepsleep(uint64_t sleep_time) {
  // スリープ前に WiFi と BT を明示的に停止しないとエラーになる
  esp_bluedroid_disable();
  esp_bt_controller_disable();
  esp_wifi_stop();
  esp_deep_sleep(1000 * 1000 * sleep_time);
}

void serial_send(float distance) {
  // モデムインスタンスを生成
  HardwareSerialModem modem(MySerial0);

  // プロトコル設定を生成
  SerialProtocol::Config config = {
      .normalTimeout = NORMALTIMEOUT,
      .postTimeout = POSTTIMEOUT,
      .url = "http://uni.soracom.io",
      .apn = "soracom.io",
  };

  // プロトコルハンドラーを生成
  SerialProtocol protocol(modem, config);

  // 距離データを送信
  protocol.sendDistance(distance, "Yokosuka-A1");
}
