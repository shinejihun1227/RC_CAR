// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include <WiFi.h>
#include <esp_now.h>
#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onReceive(const esp_now_recv_info_t *source, const uint8_t *data, int len) {
#else
void onReceive(const uint8_t *source, const uint8_t *data, int len) {
#endif
  (void)source; // 이번 실습에서는 발신자 정보 대신 명령 데이터만 사용한다.
  if (len == sizeof(uint32_t)) {
    uint32_t value;
    memcpy(&value, data, sizeof(value));
    Serial.printf("received=%lu\n", value);
  }
}
void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
}
void loop(void) {}
