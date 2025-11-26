#include "unity.h"
#include "../src/payload.hpp"

void setUp(void) {
  // 各テスト前の準備
}

void tearDown(void) {
  // 各テスト後のクリーンアップ
}

// JSON ペイロード生成のテスト

void test_buildPayload_basic(void) {
  // テスト: 基本的なペイロード生成
  String result = buildPayload(45.5, "Test-Field", 0);
  TEST_ASSERT_EQUAL_STRING("{\"distance\":45.50,\"fieldId\":\"Test-Field\"}", result.c_str());
}

void test_buildPayload_with_timestamp(void) {
  // テスト: タイムスタンプ付きペイロード生成
  String result = buildPayload(123.45, "Field-A", 1234567890);
  TEST_ASSERT_EQUAL_STRING("{\"distance\":123.45,\"fieldId\":\"Field-A\",\"ts\":1234567890}", result.c_str());
}

void test_buildPayload_zero_distance(void) {
  // テスト: 距離 0 でのペイロード生成
  String result = buildPayload(0.0, "Zero-Field", 0);
  TEST_ASSERT_EQUAL_STRING("{\"distance\":0.00,\"fieldId\":\"Zero-Field\"}", result.c_str());
}

void test_buildPayload_large_distance(void) {
  // テスト: 大きな距離値でのペイロード生成
  String result = buildPayload(9999.99, "Large-Field", 0);
  TEST_ASSERT_EQUAL_STRING("{\"distance\":9999.99,\"fieldId\":\"Large-Field\"}", result.c_str());
}

void test_buildPayload_precision(void) {
  // テスト: 浮動小数点精度（小数点第2位まで）
  String result = buildPayload(10.005, "Precision-Field", 0);
  // 丸めにより 10.01 になる
  TEST_ASSERT_EQUAL_STRING("{\"distance\":10.01,\"fieldId\":\"Precision-Field\"}", result.c_str());
}

void test_buildPayload_different_field_ids(void) {
  // テスト: 異なるフィールド ID
  String result1 = buildPayload(50.0, "Yokosuka-A1", 0);
  String result2 = buildPayload(50.0, "Yokosuka-B2", 0);

  TEST_ASSERT_EQUAL_STRING("{\"distance\":50.00,\"fieldId\":\"Yokosuka-A1\"}", result1.c_str());
  TEST_ASSERT_EQUAL_STRING("{\"distance\":50.00,\"fieldId\":\"Yokosuka-B2\"}", result2.c_str());
  TEST_ASSERT_NOT_EQUAL_STRING(result1.c_str(), result2.c_str());
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_buildPayload_basic);
  RUN_TEST(test_buildPayload_with_timestamp);
  RUN_TEST(test_buildPayload_zero_distance);
  RUN_TEST(test_buildPayload_large_distance);
  RUN_TEST(test_buildPayload_precision);
  RUN_TEST(test_buildPayload_different_field_ids);

  return UNITY_END();
}
