#include <Arduino.h>
#include "payload.hpp"

String buildPayload(float distance, const char* fieldId, unsigned long ts) {
  String payload = String("{\"distance\":") + String(distance, 2) +
                   ",\"fieldId\":\"" + fieldId + "\"";

  if (ts > 0) {
    payload += ",\"ts\":" + String(ts);
  }

  payload += "}";
  return payload;
}
