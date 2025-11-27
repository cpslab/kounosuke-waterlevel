# kounosuke-waterlevel

Seeed XIAO ESP32-C3 向けの水位監視システム。超音波センサーで距離を測定し、AT コマンドを使用して LTE モデム経由で SORACOM にデータを送信します。

## ハードウェア要件

- **MCU**: Seeed XIAO ESP32-C3
- **センサー**: 超音波距離センサー（GPIO 9/10 経由、9600 baudrate）
- **LTE モデム**: SIM7080 シリーズ（GPIO 6/7 経由、57600 baudrate）
- **電源制御**: GPIO 2 でセンサー電源スイッチを制御

## ビルドと書き込み

PlatformIO プロジェクトです。

### ビルド
```bash
pio run
```

### デバイスに書き込み
```bash
pio run -t upload
```

### シリアルモニター（57600 baud）
```bash
pio device monitor -b 57600
```

### クリーンビルド
```bash
pio run -t clean
```

## アーキテクチャ

### データフロー
1. デバイスがディープスリープから起動
2. `loop()` でシリアルコマンドを監視し、スリープ時間を動的に変更可能
3. ハードウェアシリアル（MySerial1）経由で超音波センサーから距離を読み込み
4. センサーデータを解析・検証（4 バイト プロトコル + チェックサム）
5. LTE モデム経由で HTTP POST で JSON ペイロードを SORACOM に送信
6. センサーをオフにし、設定されたスリープ時間でディープスリープに入る

### モジュール構成

#### コアモジュール

**`lib.hpp/cpp`** - メイン API
- `esp32c3_deepsleep(uint64_t sleep_time)` - ディープスリープ（WiFi/BT シャットダウン付き）
- `serial_send(float distance)` - 距離データ送信のメインエントリーポイント

**`payload.hpp/cpp`** - 純粋ビジネスロジック（テスト可能）
- `buildPayload(float distance, const char* fieldId, unsigned long ts)` - JSON ペイロード生成
- 外部依存なし、完全にユニットテスト可能

**`sensor.hpp/cpp`** - センサーデータ解析（テスト可能）
- `parseSensorData(const unsigned char* data, int& distance)` - 4 バイト超音波プロトコルを解析
- ヘッダー（0xFF）とチェックサムを検証
- 外部依存なし、完全にユニットテスト可能

**`modem.hpp/cpp`** - ハードウェア抽象化レイヤー
- `HardwareSerialModem` - Arduino HardwareSerial を使用した LTE モデム通信
- メソッド:
  - `sendCommand(const char* command, int timeout)` - AT コマンドを送信して応答を待つ
  - `sendBody(const char* bodyData)` - HTTP リクエスト用ボディデータを送信

**`serial_protocol.hpp/cpp`** - プロトコル オーケストレーション
- `SerialProtocol` - AT コマンドシーケンスを管理
- メソッド:
  - `sendDistance(float distance, const char* fieldId)` - メインエントリーポイント
  - `buildPayload(float distance, const char* fieldId, unsigned long ts)` - JSON ペイロード生成
  - `executeInitSequence()` - モデム接続を初期化
  - `executePostSequence(const char* jsonPayload)` - HTTP POST を実行
  - `executeCleanupSequence()` - 切断してクリーンアップ
- `Config` 構造体で設定可能（タイムアウト、URL、APN）
- リトライロジック実装（最大 3 回まで試行）

### データプロトコル

**超音波センサープロトコル**（MySerial1 - 9600 ボー）
```
バイト 0: 0xFF（ヘッダー）
バイト 1: 距離 MSB
バイト 2: 距離 LSB
バイト 3: チェックサム = (バイト0 + バイト1 + バイト2) & 0xFF
```

**SORACOM HTTP エンドポイント**
- URL: `http://uni.soracom.io`
- メソッド: POST
- Content-Type: application/json
- ペイロード: `{"distance": X.XX, "fieldId": "Yokosuka-A1"}`

### 設定

`main.cpp` の主な定数:
- `DEFAULT_SLEEPTIME_SECONDS = 900` - デフォルトディープスリープ時間（15 分）
- `MIN_SLEEPTIME_SECONDS = 3` - スリープ時間最小値（3 秒）
- `MAX_SLEEPTIME_SECONDS = 3600` - スリープ時間最大値（1 時間）
- `PORTLATE = 57600` - LTE モデムのボーレート
- `NORMALTIMEOUT = 5000` - 標準 AT コマンドタイムアウト（ミリ秒）
- `POSTTIMEOUT = 60000` - HTTP POST タイムアウト（ミリ秒）
- `SWITCH_PIN = 2` - センサー電源制御の GPIO ピン

### ディープスリープ時間の動的変更

実行中にシリアルコマンドを通じてディープスリープ時間を変更できます。

```bash
pio device monitor -b 57600
```

シリアルコンソールで秒単位の数値を入力すると、次回のディープスリープからその時間が適用されます：

```
3      # → 3 秒に設定
300    # → 5 分に設定
90000  # → 25 時間に設定（最大値 3600 秒を超えるとエラー）
```

- 入力値は秒単位の数値のみ受け付けます
- 最小値: 3 秒、最大値: 3600 秒（1 時間）
- 無効な入力はエラーメッセージが表示されます
- スリープ時間は RTC メモリに保存され、デバイス再起動後も保持されます

## 変更履歴

変更履歴の詳細は **[CHANGELOG.md](CHANGELOG.md)** を参照してください。
