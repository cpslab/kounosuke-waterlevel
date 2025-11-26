# ユニットテストガイド

このプロジェクトは PlatformIO の統合テスト機能と Unity テストフレームワークを使用しています。

## テストの実行

### 基本的な使用方法

```bash
# すべてのテストを実行
pio test

# PC 上で高速に実行（デバッグ用）
pio test -e seeed_xiao_esp32c3

# 特定のテストを実行
pio test -f test_payload
pio test -f test_sensor
pio test -f test_serial_protocol

# 詳細な出力を確認
pio test --verbose

# テストリストを表示（実行なし）
pio test --list-tests

# JSON 形式で結果を出力（CI/CD 用）
pio test --json-output

# JUnit XML 形式で結果を出力（GitHub Actions など）
pio test --junit-output-path=test-results.xml
```

## テストディレクトリ構造

```
test/
├── test_payload/
│   └── test_payload.cpp           # JSON ペイロード生成テスト（6 ケース）
├── test_sensor/
│   └── test_sensor.cpp            # センサーデータ解析テスト（7 ケース）
├── test_serial_protocol/
│   └── test_serial_protocol.cpp   # プロトコルロジックテスト（7 ケース）
└── common/
    ├── mock_modem.hpp             # モック モデムインターフェース
    └── mock_modem.cpp             # モック モデム実装
```

## テストスイート

### test_payload（6 テストケース）

`buildPayload()` 関数の JSON ペイロード生成をテスト

- `test_buildPayload_basic` - 基本的なペイロード生成
- `test_buildPayload_with_timestamp` - タイムスタンプ付きペイロード
- `test_buildPayload_zero_distance` - 距離 0 での生成
- `test_buildPayload_large_distance` - 大きな距離値での生成
- `test_buildPayload_precision` - 浮動小数点精度（小数点第2位）
- `test_buildPayload_different_field_ids` - 複数フィールド ID

### test_sensor（7 テストケース）

`parseSensorData()` 関数のセンサーデータ解析をテスト

- `test_parseSensorData_valid_data` - 有効なセンサーデータの解析
- `test_parseSensorData_invalid_header` - 無効なヘッダーの検出
- `test_parseSensorData_invalid_checksum` - チェックサムエラーの検出
- `test_parseSensorData_zero_distance` - 距離 0 の解析
- `test_parseSensorData_max_distance` - 最大距離値（65535 cm）
- `test_parseSensorData_multiple_values` - 複数値の一括テスト
- `test_parseSensorData_msb_lsb_order` - MSB/LSB バイト順序の検証

### test_serial_protocol（7 テストケース）

`SerialProtocol::sendDistance()` のプロトコルロジックをテスト

- `test_sendDistance_success` - 正常なデータ送信
- `test_sendDistance_init_failure` - 初期化失敗とリトライ
- `test_sendDistance_post_failure` - POST 失敗とリトライ
- `test_sendDistance_retry_count` - リトライ回数の検証
- `test_sendDistance_different_field_ids` - 複数フィールド ID での送信
- `test_sendDistance_various_distances` - 様々な距離値での送信
- `test_sendDistance_cleanup_failure` - クリーンアップ失敗の検出

## Mock モデム

`MockModem` クラスは `ModemInterface` の実装で、テスト中に AT コマンドのシーケンスを制御できます。

### 使用例

```cpp
MockModem mock_modem;

// すべてのコマンドを成功させる
mock_modem.setShouldFail(false);

// 特定のコマンド（4 番目）で失敗させる
mock_modem.setFailOnCommand(3);

// 送信されたコマンド数を取得
int count = mock_modem.getCommandCount();

// リセット
mock_modem.reset();
```

## CI/CD への統合

### GitHub Actions の例

```yaml
name: Run Unit Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Set up PlatformIO
        uses: platformio/setup-platformio-action@v1

      - name: Run tests
        run: pio test --junit-output-path=test-results.xml

      - name: Publish test results
        uses: EnricoMi/publish-unit-test-result-action@v1
        if: always()
        with:
          files: test-results.xml
```

## よくある質問

### Q: テストをビルドなしで実行できますか？

はい：
```bash
pio test --without-building
```

### Q: 特定の環境でテストを実行できますか？

はい。`platformio.ini` に複数の環境を定義できます：

```ini
[env:seeed_xiao_esp32c3]
test_framework = unity

[env:native]
platform = native
test_framework = unity
```

その後：
```bash
pio test -e native      # PC で実行
pio test -e seeed_xiao_esp32c3  # ESP32 実機で実行（オプション）
```

### Q: テストに Arduino ライブラリを使用できますか？

はい。ESP32 環境ではすべての Arduino ライブラリが利用可能です。native 環境では使用できません。

### Q: テストの出力をフィルタリングできますか？

はい：
```bash
pio test -f "test_payload"     # test_payload のみ
pio test -f "valid|invalid"    # 複数のパターンマッチ
pio test -i "cleanup"          # cleanup を除外
```
