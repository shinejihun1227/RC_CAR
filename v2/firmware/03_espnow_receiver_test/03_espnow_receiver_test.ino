#include <WiFi.h>
#include <esp_now.h>
void onReceive(const esp_now_recv_info_t*, const uint8_t* data, int len) {
  if (len == sizeof(uint32_t)) { uint32_t value; memcpy(&value, data, sizeof(value)); Serial.printf("received=%lu\n", value); }
}
void setup() { Serial.begin(115200); WiFi.mode(WIFI_STA); esp_now_init(); esp_now_register_recv_cb(onReceive); }
void loop() {}
