#include <Arduino.h>
#include "modem.hpp"

bool HardwareSerialModem::sendCommand(const char* command, int timeout) {
  serial_.write(command);
  serial_.flush();
  delay(5000);  // 応答を待つ

  while (serial_.available()) {
    String response = serial_.readStringUntil('\n');
    Serial.println(response);

    // エラーをチェック
    if (response.indexOf("ERROR") != -1) {
      return false;
    }
  }

  return true;
}

bool HardwareSerialModem::sendBody(const char* bodyData) {
  serial_.write("AT+SHBOD=1024,10000\r\n");
  serial_.flush();
  delay(1000);

  serial_.write(bodyData);
  serial_.flush();

  String response = serial_.readStringUntil('\n');
  Serial.println(response);
  response += serial_.readStringUntil('\n');

  String temp;
  do {
    temp = serial_.readStringUntil('\n');
    delay(1000);
    response += temp;
  } while (temp == "OK" || temp == "ERROR" || temp == "");
  delay(3000);

  // エラーをチェックして応答を出力
  if (response.indexOf("ERROR") != -1) {
    Serial.println("Error in response");
    return false;
  } else {
    Serial.println(response);
    return true;
  }
}
