#include "sensor.hpp"

bool parseSensorData(const unsigned char* data, int& distance) {
  // ヘッダーを検証
  if (data[0] != 0xff) {
    return false;
  }

  // チェックサムを計算して検証
  int sum = (data[0] + data[1] + data[2]) & 0x00FF;
  if (sum != data[3]) {
    return false;
  }

  // 距離を解析（data[1] は MSB、data[2] は LSB）
  distance = (data[1] << 8) + data[2];
  return true;
}
