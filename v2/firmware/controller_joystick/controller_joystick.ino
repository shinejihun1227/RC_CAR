// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 차량 ESP32의 STA MAC 주소를 아래 배열에 입력한다.
#include <WiFi.h>
#include <esp_now.h>
const int JOY_X = 34, JOY_Y = 35, STOP_BUTTON = 4;
uint8_t vehicleMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// 송신기와 수신기의 필드 순서·자료형을 동일하게 유지한다.
typedef struct {
  int16_t throttle;
  int16_t steering;
  bool emergencyStop;
  uint32_t sequence;
} ControlPacket;
uint32_t sequenceNumber = 0;
int16_t axisToCommand(int raw) {
  if (abs(raw - 2048) < 120) {
    return 0;
  }
  return (int16_t)map(raw, 0, 4095, -255, 255);
}
void setup(void) {
  Serial.begin(115200);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_peer_info_t peer = {0};
  memcpy(peer.peer_addr, vehicleMac, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}
void loop(void) {
  ControlPacket packet = {0};
  packet.throttle = axisToCommand(analogRead(JOY_Y));
  packet.steering = axisToCommand(analogRead(JOY_X));
  packet.emergencyStop = !digitalRead(STOP_BUTTON);
  packet.sequence = sequenceNumber++;
  // &는 주소, sizeof는 전송할 바이트 수를 구한다.
  esp_now_send(vehicleMac, (const uint8_t *)&packet, sizeof(packet));
  delay(30);
}
