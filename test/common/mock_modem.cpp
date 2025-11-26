#include "mock_modem.hpp"

bool MockModem::sendCommand(const char* command, int timeout) {
  // 特定のコマンドで失敗させることができる
  if (fail_on_command_ >= 0 && command_count_ == fail_on_command_) {
    command_count_++;
    return false;
  }

  // グローバルなフラグで失敗させることができる
  if (should_fail_) {
    command_count_++;
    return false;
  }

  command_count_++;
  return true;
}

bool MockModem::sendBody(const char* bodyData) {
  // グローバルなフラグで失敗させることができる
  if (should_fail_) {
    return false;
  }

  return true;
}
