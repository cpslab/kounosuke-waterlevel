# ユニットテストガイド

このプロジェクトは Unity テストフレームワークを使用したユニットテストを含んでいます。

## 環境セットアップ

### 必要なツール
- CMake 3.12 以上
- C++ コンパイラ（GCC または Clang）
- Unity テストフレームワーク

### Unity のインストール

```bash
# Unity テストフレームワークのリポジトリをクローン
git clone https://github.com/ThrowTheSwitch/Unity.git unity
```

## テストの実行

### ビルド
```bash
mkdir build
cd build
cmake ..
make
```

### テスト実行
```bash
# すべてのテストを実行
ctest

# または個別にテストを実行
./test_payload
./test_sensor
./test_serial_protocol
```

### 詳細な出力を確認
```bash
ctest --verbose
```

## テストスイート

### test_payload.cpp
`buildPayload()` 関数の JSON 生成ロジックをテスト

**テストケース:**
- `test_buildPayload_basic` - 基本的なペイロード生成
- `test_buildPayload_with_timestamp` - タイムスタンプ付きペイロード生成
- `test_buildPayload_zero_distance` - 距離 0 での生成
- `test_buildPayload_large_distance` - 大きな距離値での生成
- `test_buildPayload_precision` - 浮動小数点精度（小数点第2位）
- `test_buildPayload_different_field_ids` - 複数フィールド ID での検証

### test_sensor.cpp
`parseSensorData()` 関数のセンサーデータ解析をテスト

**テストケース:**
- `test_parseSensorData_valid_data` - 有効なセンサーデータの解析
- `test_parseSensorData_invalid_header` - 無効なヘッダーの検出
- `test_parseSensorData_invalid_checksum` - チェックサムエラーの検出
- `test_parseSensorData_zero_distance` - 距離 0 の解析
- `test_parseSensorData_max_distance` - 最大距離値（65535 cm）の解析
- `test_parseSensorData_multiple_values` - 複数値の一括テスト
- `test_parseSensorData_msb_lsb_order` - MSB/LSB バイト順序の検証

### test_serial_protocol.cpp
`SerialProtocol::sendDistance()` のプロトコルロジックをテスト

**テストケース:**
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
mock_modem.setShouldFail(false);  // すべてのコマンドを成功させる
mock_modem.setFailOnCommand(3);   // 4 番目のコマンドで失敗
int count = mock_modem.getCommandCount();  // 送信されたコマンド数を取得
```

## テスト実行時のトラブルシューティング

### Unity がインストールされていない
```bash
# unity ディレクトリが必要です
git clone https://github.com/ThrowTheSwitch/Unity.git unity
```

### CMake がユーザー定義のモジュールを見つけられない
キャッシュをクリアして再度ビルド:
```bash
rm -rf build
mkdir build
cd build
cmake ..
make
```

### Arduino 依存の問題

一部のテストは Arduino ライブラリに依存しています。
- `String` クラス
- `Serial` オブジェクト

スタンドアロン環境での実行にはモック実装が必要な場合があります。

## CI/CD への統合

GitHub Actions などで使用する場合:

```yaml
- name: Run Tests
  run: |
    mkdir build && cd build
    cmake ..
    make
    ctest --verbose
```
