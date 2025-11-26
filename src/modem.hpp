#ifndef MODEM_HPP
#define MODEM_HPP

/**
 * モデム通信のための抽象インターフェース
 * テスト用の依存性注入を可能にする
 */
class ModemInterface {
 public:
  virtual ~ModemInterface() = default;

  /**
   * AT コマンドを送信して応答を待つ
   * @param command AT コマンド文字列（例："AT+CFUN=6\r\n"）
   * @param timeout タイムアウト（ミリ秒）
   * @return 成功時（応答に ERROR がない）は true、それ以外は false
   */
  virtual bool sendCommand(const char* command, int timeout) = 0;

  /**
   * HTTP リクエスト用のボディデータを送信
   * @param bodyData ボディの内容
   * @return 成功時は true、失敗時は false
   */
  virtual bool sendBody(const char* bodyData) = 0;
};

/**
 * HardwareSerial を使用したモデムの具体的実装
 */
class HardwareSerialModem : public ModemInterface {
 public:
  HardwareSerialModem(HardwareSerial& serial) : serial_(serial) {}

  bool sendCommand(const char* command, int timeout) override;
  bool sendBody(const char* bodyData) override;

 private:
  HardwareSerial& serial_;
};

#endif // MODEM_HPP
