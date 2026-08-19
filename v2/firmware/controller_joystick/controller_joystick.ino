// 차량 ESP32의 STA MAC 주소를 아래 배열에 입력한다.
#include <WiFi.h>
#include <esp_now.h>
constexpr int JOY_X = 34, JOY_Y = 35, STOP_BUTTON = 4;
uint8_t vehicleMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
struct ControlPacket { int16_t throttle, steering; bool emergencyStop; uint32_t sequence; };
uint32_t sequenceNumber = 0;
int16_t axisToCommand(int raw) { return abs(raw - 2048) < 120 ? 0 : map(raw, 0, 4095, -255, 255); }
void setup() {
  Serial.begin(115200); pinMode(STOP_BUTTON, INPUT_PULLUP);
  WiFi.mode(WIFI_STA); esp_now_init();
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, vehicleMac, 6); peer.channel = 0; peer.encrypt = false;
  esp_now_add_peer(&peer);
}
void loop() {
  ControlPacket packet{axisToCommand(analogRead(JOY_Y)), axisToCommand(analogRead(JOY_X)), !digitalRead(STOP_BUTTON), sequenceNumber++};
  esp_now_send(vehicleMac, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
  delay(30);
}
