#include <Arduino.h>
#include <SPI.h>

#include "driver/gpio.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"
#include "lib.hpp"

// ハードウェアシリアルの準備
HardwareSerial MySerial0(0);
HardwareSerial MySerial1(1);

const int SWITCH_PIN = 2; // Xiao C3のGPIO2ピンを使用

// coprocessor領域に変数を宣言することでスリープ復帰後も値が保持できる
RTC_DATA_ATTR int counter = 0; // RTC

const uint64_t SLEEPTIME_SECONDS = 900; // 秒(3600→1時間)

const int PORTLATE = 57600;
const int BIGTIMEOUT = 10000;
int POSTTIMEOUT = 60000;
int NORMALTIMEOUT = 5000;
const int SMALLTIMEOUT = 1000;

unsigned char data[4] = {};

int count = 0;

float distance = -1;

int failureCount;

void setup() {
  Serial.begin(PORTLATE);
  // Configure MySerial0 on pins TX=6 and RX=7 (-1, -1 means use the default)
  MySerial0.begin(PORTLATE, SERIAL_8N1, -1, -1);
  MySerial1.begin(9600, SERIAL_8N1, 9, 10);
  pinMode(SWITCH_PIN, OUTPUT); // ピンを出力として設定
  digitalWrite(SWITCH_PIN, HIGH);
  distance = -1;
  count = 0;
  failureCount = 0; // Counter to track consecutive failures
}

void loop() {
  do {
    for (int i = 0; i < 4; i++) {
      data[i] = MySerial1.read();
    }
  } while (MySerial1.read() == 0xff);

  MySerial1.flush();

  if (data[0] == 0xff) {
    int sum;
    sum = (data[0] + data[1] + data[2]) & 0x00FF;
    if (sum == data[3]) {
      distance = (data[1] << 8) + data[2];
      if (distance > 30) {
        Serial.print("distance=");
        Serial.print(distance / 10);
        Serial.println("cm");
      } else {
        Serial.println("Below the lower limit");
      }
    } else
      Serial.println("ERROR");
  }
  delay(100);
  count += 1;

  if (count > 100 || distance != -1) // デバッグで＆から変更
  {
    // delay(5000);//シリアルコンソール確認用のdelay(本番では不要)
    Serial.println("start");
    serial_send(distance / 10);
    digitalWrite(SWITCH_PIN, LOW); // センサ類をOFFにする
    esp32c3_deepsleep(
        SLEEPTIME_SECONDS); // スリープタイム
                            // スリープ中にGPIO2がHIGHになったら目覚める
  }
}
