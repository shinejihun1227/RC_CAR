#include <WiFi.h>
#include <esp_now.h>
uint8_t vehicleMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 차량 MAC으로 교체
uint32_t sequence = 0;
void setup() {
  Serial.begin(115200); WiFi.mode(WIFI_STA); esp_now_init();
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, vehicleMac, 6); peer.channel = 0;
  esp_now_add_peer(&peer);
}
void loop() {
  esp_now_send(vehicleMac, reinterpret_cast<uint8_t*>(&sequence), sizeof(sequence));
  Serial.printf("sent=%lu\n", sequence++); delay(500);
}
