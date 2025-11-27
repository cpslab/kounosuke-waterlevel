#ifndef MODEM_HPP
#define MODEM_HPP

#include <HardwareSerial.h>

/**
 * HardwareSerial を使用したモデムの具体的実装
 */
class HardwareSerialModem {
public:
  HardwareSerialModem(HardwareSerial &serial) : serial_(serial) {}

  bool sendCommand(const char *command, int timeout);
  bool sendBody(const char *bodyData);

private:
  HardwareSerial &serial_;
};

#endif // MODEM_HPP
