cmake_minimum_required(VERSION 3.12)
project(kounosuke-waterlevel-tests)

# C++ 標準を設定
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Unity テストフレームワークを追加
add_subdirectory(unity)

# テスト対象のソースファイル
set(SOURCE_FILES
    src/payload.cpp
    src/sensor.cpp
    src/serial_protocol.cpp
    test/mock_modem.cpp
)

# ペイロードテスト
add_executable(test_payload test/test_payload.cpp ${SOURCE_FILES})
target_include_directories(test_payload PRIVATE .)
target_link_libraries(test_payload unity)

# センサーテスト
add_executable(test_sensor test/test_sensor.cpp ${SOURCE_FILES})
target_include_directories(test_sensor PRIVATE .)
target_link_libraries(test_sensor unity)

# プロトコルテスト
add_executable(test_serial_protocol test/test_serial_protocol.cpp ${SOURCE_FILES})
target_include_directories(test_serial_protocol PRIVATE .)
target_link_libraries(test_serial_protocol unity)

# テストを有効化
enable_testing()
add_test(NAME Payload COMMAND test_payload)
add_test(NAME Sensor COMMAND test_sensor)
add_test(NAME SerialProtocol COMMAND test_serial_protocol)
