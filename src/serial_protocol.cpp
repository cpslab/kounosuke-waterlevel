#include "serial_protocol.hpp"
#include <Arduino.h>

SerialProtocol::SerialProtocol(HardwareSerialModem &modem, const Config &config)
    : modem_(modem), config_(config) {}

bool SerialProtocol::sendDistance(float distance, const char *fieldId) {
  int failureCount = 0;

  while (failureCount < 3) {
    if (!executeInitSequence()) {
      Serial.println("初期化シーケンス失敗");
      failureCount++;
      continue;
    }

    // ペイロードを生成
    String payload = buildPayload(distance, fieldId);

    if (!executePostSequence(payload.c_str())) {
      Serial.println("POST シーケンス失敗");
      failureCount++;
      continue;
    }

    if (!executeCleanupSequence()) {
      Serial.println("クリーンアップシーケンス失敗");
      failureCount++;
      continue;
    }

    // 成功
    Serial.println("完了");
    return true;
  }

  Serial.println("3回連続でエラーが発生しました。おやすみなさい。");
  return false;
}

bool SerialProtocol::executeInitSequence() {
  if (!modem_.sendCommand("AT+CFUN=6\r\n", config_.normalTimeout)) {
    Serial.println("エラー: AT+CFUN=6");
    return false;
  }

  if (!modem_.sendCommand("AT+CGDCONT=1,\"IP\",\"soracom.io\"\r\n",
                          config_.normalTimeout)) {
    Serial.println("エラー: AT+CGDCONT=1");
    return false;
  }

  if (!modem_.sendCommand("AT+CNACT=0,1\r\n", config_.normalTimeout)) {
    Serial.println("エラー: AT+CNACT");
    return false;
  }

  return true;
}

bool SerialProtocol::executePostSequence(const char *jsonPayload) {
  if (!modem_.sendCommand("AT+SHCONF=\"URL\",\"http://uni.soracom.io\"\r\n",
                          config_.normalTimeout)) {
    Serial.println("エラー: AT+SHCONF URL");
    return false;
  }

  if (!modem_.sendCommand("AT+SHCONF=\"BODYLEN\",1024\r\n",
                          config_.normalTimeout)) {
    Serial.println("エラー: AT+SHCONF BODYLEN");
    return false;
  }

  if (!modem_.sendCommand("AT+SHCONF=\"HEADERLEN\",350\r\n",
                          config_.normalTimeout)) {
    Serial.println("エラー: AT+SHCONF HEADERLEN");
    return false;
  }

  if (!modem_.sendCommand("AT+SHCONN\r\n", config_.normalTimeout)) {
    Serial.println("エラー: AT+SHCONN");
    return false;
  }

  if (!modem_.sendCommand(
          "AT+SHAHEAD=\"Content-Type\",\"application/json\"\r\n",
          config_.normalTimeout)) {
    Serial.println("エラー: AT+SHAHEAD");
    return false;
  }

  if (!modem_.sendBody(jsonPayload)) {
    Serial.println("エラー: JSON データ");
    return false;
  }

  if (!modem_.sendCommand("AT+SHREQ=\"http://uni.soracom.io\",3\r\n",
                          config_.postTimeout)) {
    Serial.println("エラー: AT+SHREQ");
    return false;
  }

  return true;
}

bool SerialProtocol::executeCleanupSequence() {
  if (!modem_.sendCommand("AT+SHDISC\r\n", config_.normalTimeout)) {
    Serial.println("エラー: AT+SHDISC");
    return false;
  }

  return true;
}

String SerialProtocol::buildPayload(float distance, const char *fieldId,
                                    unsigned long ts) {
  String payload = String("{\"distance\":") + String(distance, 2) +
                   ",\"fieldId\":\"" + fieldId + "\"";

  if (ts > 0) {
    payload += ",\"ts\":" + String(ts);
  }

  payload += "}";
  return payload;
}
