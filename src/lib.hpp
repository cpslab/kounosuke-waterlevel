#ifndef LIB_HPP
#define LIB_HPP

#include <cstdint>

// ディープスリープ関数
void esp32c3_deepsleep(uint64_t sleep_time);

// メインのシリアル通信関数
void serial_send(float distance);

#endif // LIB_HPP
