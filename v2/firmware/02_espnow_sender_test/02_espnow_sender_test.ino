// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include <WiFi.h>
#include <esp_now.h>
uint8_t vehicleMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 차량 MAC으로 교체
uint32_t sequence = 0;
void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_peer_info_t peer = {0};
  memcpy(peer.peer_addr, vehicleMac, 6);
  peer.channel = 0;
  esp_now_add_peer(&peer);
}
void loop(void) {
  esp_now_send(vehicleMac, (const uint8_t *)&sequence, sizeof(sequence));
  Serial.printf("sent=%lu\n", sequence++);
  delay(500);
}
