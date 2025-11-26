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
2. ハードウェアシリアル（MySerial1）経由で超音波センサーから距離を読み込み
3. センサーデータを解析・検証（4 バイト プロトコル + チェックサム）
4. LTE モデム経由で HTTP POST で JSON ペイロードを SORACOM に送信
5. センサーをオフにし、15 分間のディープスリープに入る

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

**`modem.hpp/cpp`** - ハードウェア抽象化レイヤー（依存性注入）
- `ModemInterface` - モデム通信の抽象基底クラス
- `HardwareSerialModem` - Arduino HardwareSerial を使用した具体的実装
- ユニットテストのためのモック化が容易

**`serial_protocol.hpp/cpp`** - プロトコル オーケストレーション（部分的にテスト可能）
- `SerialProtocol` - AT コマンドシーケンスを管理
- メソッド:
  - `sendDistance(float distance, const char* fieldId)` - メインエントリーポイント
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
- `SLEEPTIME_SECONDS = 900` - ディープスリープ時間（15 分）
- `PORTLATE = 57600` - LTE モデムのボーレート
- `NORMALTIMEOUT = 5000` - 標準 AT コマンドタイムアウト（ミリ秒）
- `POSTTIMEOUT = 60000` - HTTP POST タイムアウト（ミリ秒）
- `SWITCH_PIN = 2` - センサー電源制御の GPIO ピン

## テスト

モジュール化されたアーキテクチャにより、ユニットテストが可能です。プロジェクトには **Unity** テストフレームワークを使用した包括的なテストスイートが含まれています。

### ユニットテストの実行

詳細は **[TESTING.md](TESTING.md)** を参照してください。

```bash
# セットアップ
git clone https://github.com/ThrowTheSwitch/Unity.git unity
mkdir build && cd build
cmake ..
make

# テスト実行
ctest --verbose
```

### テストファイル

- **`test/test_payload.cpp`** - JSON ペイロード生成のテスト（6 テストケース）
- **`test/test_sensor.cpp`** - センサーデータ解析のテスト（7 テストケース）
- **`test/test_serial_protocol.cpp`** - プロトコルロジックのテスト（7 テストケース）

### テスト可能なコンポーネント

**純粋ロジック（ハードウェア独立）:**
- `buildPayload()` - JSON ペイロード生成
- `parseSensorData()` - センサーデータ解析とチェックサム検証

**依存性注入（モック化可能）:**
- `SerialProtocol` - AT コマンドシーケンス（`MockModem` でテスト）

### テスト構造の例

```cpp
// テスト用のモック モデム
class MockModem : public ModemInterface {
  bool sendCommand(const char* command, int timeout) override { /* ... */ }
  bool sendBody(const char* bodyData) override { /* ... */ }
};

// モックでプロトコルをテスト
MockModem mock;
SerialProtocol::Config config = { /* ... */ };
SerialProtocol protocol(mock, config);
bool result = protocol.sendDistance(45.5, "Test-Field");
```

### テストカバレッジ

- ✅ 有効・無効なデータの入力
- ✅ エッジケース（距離 0、最大値）
- ✅ チェックサム検証
- ✅ リトライロジック（最大 3 回）
- ✅ タイムスタンプ付き/なしペイロード
- ✅ 複数フィールド ID

## 変更履歴

変更履歴の詳細は **[CHANGELOG.md](CHANGELOG.md)** を参照してください。
