#ifndef SENSOR_HPP
#define SENSOR_HPP

/**
 * 超音波センサーデータを検証して解析
 * プロトコル: 4 バイト: 0xFF、MSB、LSB、チェックサム
 *
 * @param data センサーからの 4 バイト配列
 * @param distance 解析された距離の出力パラメーター
 * @return 有効な場合は true、チェックサムエラーの場合は false
 */
bool parseSensorData(const unsigned char* data, int& distance);

#endif // SENSOR_HPP
