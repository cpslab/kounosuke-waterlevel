#include <Arduino.h>
#include <cstdint>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"
#include "esp_sleep.h"

extern HardwareSerial MySerial0;
extern int NORMALTIMEOUT;
extern int POSTTIMEOUT;

void esp32c3_deepsleep(uint64_t sleep_time) {
  // スリープ前にwifiとBTを明示的に止めないとエラーになる
  esp_bluedroid_disable();
  esp_bt_controller_disable();
  esp_wifi_stop();
  esp_deep_sleep(1000 * 1000 * sleep_time);
}

bool sendATCommand(const char *command, const int timeout) {
  MySerial0.write(command);
  MySerial0.flush();
  delay(5000); // 応答を待つための適切な遅延を設定

  while (MySerial0.available()) {
    String response = MySerial0.readStringUntil('\n');
    Serial.println(response);

    // エラーチェック
    if (response.indexOf("ERROR") != -1) {
      return false; // エラーが検出された場合
    }
  }

  return true;
}

bool sendBody(const char *command) {
  MySerial0.write("AT+SHBOD=1024,10000\r\n");
  MySerial0.flush();
  delay(1000);
  MySerial0.write(command);
  MySerial0.flush();
  String response = MySerial0.readStringUntil('\n');
  Serial.println(response);
  response += MySerial0.readStringUntil('\n');
  String temp;
  do {
    temp = MySerial0.readStringUntil('\n');
    delay(1000);
    response += temp;
  } while (temp == "OK" || temp == "ERROR" || temp == "");
  delay(3000);

  // エラーチェックとテキスト形式のレスポンスの出力
  if (response.indexOf("ERROR") != -1) {
    Serial.println("Error in response");
    return false;
  } else {
    Serial.println(response);
    return true;
  }
}

void serial_send(float distance) {
  int failureCount = 0;

  while (failureCount < 3) {
    if (!sendATCommand("AT+CFUN=6\r\n", NORMALTIMEOUT)) {
      Serial.println("Error: AT+CFUN=6");
      failureCount++;
      continue;
    }
    // delay(3000);

    if (!sendATCommand("AT+CGDCONT=1,\"IP\",\"soracom.io\"\r\n",
                       NORMALTIMEOUT)) {
      Serial.println("Error: AT+CGDCONT=1");
      failureCount++;
      continue;
    }
    // delay(1000);

    if (!sendATCommand("AT+CNACT=0,1\r\n", NORMALTIMEOUT)) {
      Serial.println("Error: AT+CNACT");
      failureCount++;
      continue;
    }
    // delay(5000);

    if (!sendATCommand("AT+SHCONF=\"URL\",\"http://uni.soracom.io\"\r\n",
                       NORMALTIMEOUT)) {
      Serial.println("Error: AT+SHCONF URL");
      failureCount++;
      continue;
    }
    // delay(1000);

    if (!sendATCommand("AT+SHCONF=\"BODYLEN\",1024\r\n", NORMALTIMEOUT)) {
      Serial.println("Error: AT+SHCONF BODYLEN");
      failureCount++;
      continue;
    }
    // delay(1000);

    if (!sendATCommand("AT+SHCONF=\"HEADERLEN\",350\r\n", NORMALTIMEOUT)) {
      Serial.println("Error: AT+SHCONF HEADERLEN");
      failureCount++;
      continue;
    }
    // delay(1000);

    if (!sendATCommand("AT+SHCONN\r\n", NORMALTIMEOUT)) {
      Serial.println("Error: AT+SHCONN");
      failureCount++;
      continue;
    }
    // delay(1000);

    if (!sendATCommand("AT+SHAHEAD=\"Content-Type\",\"application/json\"\r\n",
                       NORMALTIMEOUT)) {
      Serial.println("Error: AT+SHAHEAD");
      failureCount++;
      continue;
    }
    // delay(1000);

    // ------- Funnel→AWS IoT ルール用のJSONペイロード -------
    String fieldId = "Yokosuka-A1"; // 設置場所など任意
    unsigned long ts =
        millis(); // 端末時刻を使うなら IoTルール側で ${ts} を指定

    String payload = String("{\"distance\":") + String(distance, 2) +
                     ",\"fieldId\":\"" + fieldId +
                     "\""
                     // 端末時刻も使いたい場合は下1行のコメントを外す
                     // + ",\"ts\":" + String(ts)
                     + "}";

    if (!sendBody(payload.c_str())) {
      Serial.println("Error: JSON Data");
      failureCount++;
      continue;
    }

    // String distance_json = "\"distance\":" + String(distance);
    // String All_data = "{" + distance_json + "}\r\n";

    // if (!sendBody(All_data.c_str())) {
    //     Serial.println("Error: JSON Data");
    //     failureCount++;
    //     continue;
    // }
    // delay(2000);

    if (!sendATCommand("AT+SHREQ=\"http://uni.soracom.io\",3\r\n",
                       POSTTIMEOUT)) {
      Serial.println("Error: AT+SHREQ");
      failureCount++;
      continue;
    }
    // delay(2000);

    if (!sendATCommand("AT+SHDISC\r\n", NORMALTIMEOUT)) {
      Serial.println("Error: AT+SHDISC");
      failureCount++;
      continue;
    }

    // If all commands succeed, exit the loop
    break;
  }

  if (failureCount < 3) {
    Serial.println("done");
  } else {
    Serial.println("3回連続でエラーが発生しました。おやすみなさい。");
  }
}
