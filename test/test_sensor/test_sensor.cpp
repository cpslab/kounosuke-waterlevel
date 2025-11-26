#include <unity.h>
#include "../../src/sensor.hpp"

void setUp(void) {
  // 各テスト前の準備
}

void tearDown(void) {
  // 各テスト後のクリーンアップ
}

// センサーデータ解析のテスト

void test_parseSensorData_valid_data(void) {
  // テスト: 有効なセンサーデータの解析
  unsigned char data[4] = {0xFF, 0x01, 0x2C, 0xFB};
  // チェックサム: (0xFF + 0x01 + 0x2C) & 0xFF = 0xFB
  // 距離: (0x01 << 8) + 0x2C = 256 + 44 = 300
  int distance = -1;

  bool result = parseSensorData(data, distance);

  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT(300, distance);
}

void test_parseSensorData_invalid_header(void) {
  // テスト: 無効なヘッダー（0xFF 以外）
  unsigned char data[4] = {0xFE, 0x01, 0x2C, 0xFA};
  int distance = -1;

  bool result = parseSensorData(data, distance);

  TEST_ASSERT_FALSE(result);
}

void test_parseSensorData_invalid_checksum(void) {
  // テスト: 無効なチェックサム
  unsigned char data[4] = {0xFF, 0x01, 0x2C, 0xFC};  // 正しくは 0xFB
  int distance = -1;

  bool result = parseSensorData(data, distance);

  TEST_ASSERT_FALSE(result);
}

void test_parseSensorData_zero_distance(void) {
  // テスト: 距離 0 の解析
  unsigned char data[4] = {0xFF, 0x00, 0x00, 0xFF};
  // チェックサム: (0xFF + 0x00 + 0x00) & 0xFF = 0xFF
  // 距離: (0x00 << 8) + 0x00 = 0
  int distance = -1;

  bool result = parseSensorData(data, distance);

  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT(0, distance);
}

void test_parseSensorData_max_distance(void) {
  // テスト: 最大距離値の解析
  unsigned char data[4] = {0xFF, 0xFF, 0xFF, 0xFD};
  // チェックサム: (0xFF + 0xFF + 0xFF) & 0xFF = 0xFD
  // 距離: (0xFF << 8) + 0xFF = 65535
  int distance = -1;

  bool result = parseSensorData(data, distance);

  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT(65535, distance);
}

void test_parseSensorData_multiple_values(void) {
  // テスト: 複数のセンサー値を解析
  struct {
    unsigned char data[4];
    int expected_distance;
  } test_cases[] = {
      {{0xFF, 0x00, 0x64, 0x63}, 100},      // 100 cm
      {{0xFF, 0x01, 0xF4, 0xF3}, 500},      // 500 cm
      {{0xFF, 0x0C, 0x80, 0x3F}, 3200},     // 3200 cm
      {{0xFF, 0x27, 0x10, 0x36}, 10000},    // 10000 cm
  };

  for (int i = 0; i < 4; i++) {
    int distance = -1;
    bool result = parseSensorData(test_cases[i].data, distance);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(test_cases[i].expected_distance, distance);
  }
}

void test_parseSensorData_msb_lsb_order(void) {
  // テスト: MSB/LSB のバイト順序が正しいことを確認
  // 距離 0x1234 = 4660 を期待
  unsigned char data[4] = {0xFF, 0x12, 0x34, 0x65};
  // チェックサム: (0xFF + 0x12 + 0x34) & 0xFF = 0x45 + 0x20 = 0x65
  int distance = -1;

  bool result = parseSensorData(data, distance);

  TEST_ASSERT_TRUE(result);
  TEST_ASSERT_EQUAL_INT(0x1234, distance);
}
