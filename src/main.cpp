#include <Arduino.h>
#include <SPI.h>

#include "lib.hpp"
#include "sensor.hpp"

/// 前方宣言
bool readSensorData();
void sendAndSleep();
bool processSensorData();

// =============== ハードウェアシリアルの設定 ===============
HardwareSerial MySerial0(0); // LTE モデム通信
HardwareSerial MySerial1(1); // 超音波センサー通信

// =============== GPIO ピン設定 ===============
const int SWITCH_PIN = 2; // センサー電源制御（GPIO 2）

// =============== 通信速度とタイムアウト ===============
const int PORTLATE = 57600; // LTE モデムのボーレート
int POSTTIMEOUT = 60000;    // HTTP POST タイムアウト（ミリ秒）
int NORMALTIMEOUT = 5000;   // AT コマンドタイムアウト（ミリ秒）

// =============== スリープ設定 ===============
const uint64_t DEFAULT_SLEEPTIME_SECONDS = 900; // デフォルト（15 分）
const uint64_t MIN_SLEEPTIME_SECONDS = 3;       // 最小値（3 秒）
const uint64_t MAX_SLEEPTIME_SECONDS = 3600;    // 最大値（1 時間）
RTC_DATA_ATTR uint64_t sleeptime_seconds = DEFAULT_SLEEPTIME_SECONDS; // RTC 領域のスリープ時間
RTC_DATA_ATTR int counter = 0;                                          // RTC 領域の変数（スリープ後も保持）

// =============== センサーデータ ===============
unsigned char sensor_data[4] = {}; // 超音波センサーの 4 バイト
int sensor_read_count = 0;         // センサー読み込み回数
float last_distance = -1;          // 最後の有効な距離値

/**
 * 初期化処理
 * シリアル通信、GPIO、グローバル変数を初期化
 */
void setup() {
  // デバッグ用シリアル
  Serial.begin(PORTLATE);

  // LTE モデム通信（MySerial0）- デフォルトピン TX=6, RX=7
  MySerial0.begin(PORTLATE, SERIAL_8N1, -1, -1);

  // 超音波センサー通信（MySerial1）- GPIO 9, 10（9600 ボー）
  MySerial1.begin(9600, SERIAL_8N1, 9, 10);

  // センサー電源制御ピンを初期化
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN, HIGH); // センサーをオン

  // グローバル変数を初期化
  sensor_read_count = 0;
  last_distance = -1;
}

/**
 * シリアルコマンドを処理
 * フォーマット: 数値（秒単位のスリープ時間）
 */
void handleSerialCommand() {
  if (!Serial.available()) {
    return;
  }

  String input = Serial.readStringUntil('\n');
  input.trim();

  // 数値かどうかを確認
  if (input.length() == 0) {
    return;
  }

  bool valid = true;
  for (int i = 0; i < input.length(); i++) {
    if (!isdigit(input[i])) {
      valid = false;
      break;
    }
  }

  if (!valid) {
    Serial.println("エラー: 数値で入力してください");
    return;
  }

  uint64_t new_sleeptime = input.toInt();

  // バリデーション
  if (new_sleeptime < MIN_SLEEPTIME_SECONDS) {
    Serial.print("エラー: 最小値は ");
    Serial.print(MIN_SLEEPTIME_SECONDS);
    Serial.println(" 秒です");
    return;
  }

  if (new_sleeptime > MAX_SLEEPTIME_SECONDS) {
    Serial.print("エラー: 最大値は ");
    Serial.print(MAX_SLEEPTIME_SECONDS);
    Serial.println(" 秒です");
    return;
  }

  sleeptime_seconds = new_sleeptime;
  Serial.print("スリープ時間を ");
  Serial.print(sleeptime_seconds);
  Serial.println(" 秒に設定しました");
}

/**
 * メインループ
 * センサーデータを読み込み、解析し、定期的にデータを送信
 */
void loop() {
  // シリアルコマンドを処理
  handleSerialCommand();

  // センサーデータを読み込む
  if (!readSensorData()) {
    delay(100);
    return;
  }

  // センサーデータを解析
  bool valid_data = processSensorData();
  delay(100);

  // 読み込み回数をインクリメント
  sensor_read_count++;

  // 以下の条件でデータ送信：
  // 1. 有効なセンサーデータを取得した、または
  // 2. 読み込み回数が 100 を超えた
  if (valid_data || sensor_read_count > 100) {
    sendAndSleep();
  }
}

/// ユーティリティ関数

/**
 * センサーデータを読み込む
 * 超音波センサーから 4 バイトデータを取得
 *
 * @return 有効なセンサーデータを取得できた場合は true
 */
bool readSensorData() {
  // 0xFF フレームヘッダーを探す
  do {
    for (int i = 0; i < 4; i++) {
      sensor_data[i] = MySerial1.read();
    }
  } while (MySerial1.read() == 0xff);

  MySerial1.flush();

  // データが無効な場合は false を返す
  if (sensor_data[0] != 0xff) {
    return false;
  }

  return true;
}

/**
 * センサーデータを解析してログ出力
 *
 * @return 有効な距離が解析できた場合は true
 */
bool processSensorData() {
  // センサーのパース関数を使用
  int distance = 0;
  if (!parseSensorData(sensor_data, distance)) {
    Serial.println("エラー: センサーデータのチェックサムが不正");
    return false;
  }

  // 距離の下限値をチェック（30 = 3.0 cm）
  if (distance <= 30) {
    Serial.println("下限値以下");
    return false;
  }

  last_distance = distance / 10.0; // センチメートル単位に変換
  Serial.print("距離=");
  Serial.print(last_distance);
  Serial.println("cm");

  return true;
}

/**
 * データ送信とスリープ
 */
void sendAndSleep() {
  Serial.println("開始");
  serial_send(last_distance);
  digitalWrite(SWITCH_PIN, LOW);        // センサーをオフ
  esp32c3_deepsleep(sleeptime_seconds); // ディープスリープ
}
