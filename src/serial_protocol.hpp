#ifndef SERIAL_PROTOCOL_HPP
#define SERIAL_PROTOCOL_HPP

#include "modem.hpp"

class String;

/**
 * SORACOM 経由でデータを送信するための AT コマンドシーケンスを管理
 */
class SerialProtocol {
public:
  struct Config {
    int normalTimeout;
    int postTimeout;
    const char *url;
    const char *apn;
  };

  SerialProtocol(HardwareSerialModem &modem, const Config &config);

  /**
   * HTTP POST 経由で距離データを送信
   * @param distance 距離値（cm）
   * @param fieldId フィールド識別子
   * @return 成功時は true、3 回連続失敗時は false
   */
  bool sendDistance(float distance, const char *fieldId);

  String buildPayload(float distance, const char *fieldId,
                      unsigned long ts = 0);

private:
  HardwareSerialModem &modem_;
  Config config_;

  // AT コマンドシーケンス実行
  bool executeInitSequence();
  bool executePostSequence(const char *jsonPayload);
  bool executeCleanupSequence();
};

#endif // SERIAL_PROTOCOL_HPP
