#include <unity.h>
#include "../../src/serial_protocol.hpp"
#include "../common/mock_modem.hpp"

MockModem mock_modem;
SerialProtocol* protocol = NULL;

void setUp(void) {
  // 各テスト前にモデムをリセット
  mock_modem.reset();

  // プロトコル設定
  SerialProtocol::Config config = {
      .normalTimeout = 5000,
      .postTimeout = 60000,
      .url = "http://uni.soracom.io",
      .apn = "soracom.io",
  };

  // プロトコルを生成
  protocol = new SerialProtocol(mock_modem, config);
}

void tearDown(void) {
  // テスト後にプロトコルを削除
  delete protocol;
  protocol = NULL;
}

// シリアルプロトコルのテスト

void test_sendDistance_success(void) {
  // テスト: 正常なデータ送信
  mock_modem.setShouldFail(false);

  bool result = protocol->sendDistance(45.5, "Test-Field");

  TEST_ASSERT_TRUE(result);
  // 初期化 3 + POST 6 + クリーンアップ 1 = 10 コマンド
  TEST_ASSERT_EQUAL_INT(10, mock_modem.getCommandCount());
}

void test_sendDistance_init_failure(void) {
  // テスト: 初期化シーケンスの失敗とリトライ
  mock_modem.setFailOnCommand(0);  // 最初のコマンドで失敗

  bool result = protocol->sendDistance(45.5, "Test-Field");

  // 3 回のリトライが行われるため、失敗する
  TEST_ASSERT_FALSE(result);
}

void test_sendDistance_post_failure(void) {
  // テスト: POST シーケンスの失敗とリトライ
  // 初期化は成功（3 コマンド）、POST シーケンスで失敗
  mock_modem.setFailOnCommand(3);  // 初期化後のコマンドで失敗

  bool result = protocol->sendDistance(45.5, "Test-Field");

  // 失敗する
  TEST_ASSERT_FALSE(result);
}

void test_sendDistance_retry_count(void) {
  // テスト: リトライ回数が正しいことを確認
  // グローバルで失敗させるので、毎回失敗する
  mock_modem.setShouldFail(true);

  bool result = protocol->sendDistance(45.5, "Test-Field");

  TEST_ASSERT_FALSE(result);
  // 3 回のリトライ × 初期化シーケンス = 3 回以上のコマンド
  TEST_ASSERT_TRUE(mock_modem.getCommandCount() >= 3);
}

void test_sendDistance_different_field_ids(void) {
  // テスト: 異なるフィールド ID での送信
  mock_modem.setShouldFail(false);

  bool result1 = protocol->sendDistance(50.0, "Field-A");
  TEST_ASSERT_TRUE(result1);

  mock_modem.reset();

  bool result2 = protocol->sendDistance(50.0, "Field-B");
  TEST_ASSERT_TRUE(result2);
}

void test_sendDistance_various_distances(void) {
  // テスト: 様々な距離値での送信
  float distances[] = {0.0, 10.5, 100.0, 9999.99};

  for (int i = 0; i < 4; i++) {
    mock_modem.reset();
    mock_modem.setShouldFail(false);

    bool result = protocol->sendDistance(distances[i], "Test-Field");
    TEST_ASSERT_TRUE(result);
  }
}

void test_sendDistance_cleanup_failure(void) {
  // テスト: クリーンアップシーケンスの失敗
  // 初期化 3 + POST 6 = 9 コマンド後でクリーンアップが失敗
  mock_modem.setFailOnCommand(9);

  bool result = protocol->sendDistance(45.5, "Test-Field");

  // 失敗する
  TEST_ASSERT_FALSE(result);
}
