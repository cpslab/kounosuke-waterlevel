#ifndef MOCK_MODEM_HPP
#define MOCK_MODEM_HPP

#include "../../src/modem.hpp"

/**
 * テスト用のモック モデム実装
 * テスト中に AT コマンドのシーケンスを制御できる
 */
class MockModem : public ModemInterface {
 public:
  MockModem() : command_count_(0), should_fail_(false), fail_on_command_(-1) {}

  bool sendCommand(const char* command, int timeout) override;
  bool sendBody(const char* bodyData) override;

  // テスト用ヘルパーメソッド
  void setShouldFail(bool should_fail) { should_fail_ = should_fail; }
  void setFailOnCommand(int command_index) { fail_on_command_ = command_index; }
  int getCommandCount() const { return command_count_; }
  void reset() {
    command_count_ = 0;
    should_fail_ = false;
    fail_on_command_ = -1;
  }

 private:
  int command_count_;
  bool should_fail_;
  int fail_on_command_;
};

#endif // MOCK_MODEM_HPP
