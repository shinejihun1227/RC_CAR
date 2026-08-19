// 필요 라이브러리: SparkFun BMI270 Arduino Library
// 이 파일은 v2의 ControlPacket과 ESP-NOW 송신부를 재사용한다.
#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <SparkFun_BMI270_Arduino_Library.h>

constexpr int STOP_BUTTON = 4;
constexpr float DEAD_ZONE_DEG = 8.0f, FULL_SCALE_DEG = 35.0f;
uint8_t vehicleMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
BMI270 bmi;
struct ControlPacket { int16_t throttle, steering; bool emergencyStop; uint32_t sequence; };
float pitchZero = 0, rollZero = 0; uint32_t sequenceNumber = 0;

int16_t angleToCommand(float degree) {
  if (abs(degree) < DEAD_ZONE_DEG) return 0;
  return constrain((int)((degree / FULL_SCALE_DEG) * 255), -255, 255);
}
void calibrateNeutral() {
  float rollSum = 0, pitchSum = 0;
  for (int i = 0; i < 100; ++i) {
    bmi.getSensorData();
    rollSum += atan2(bmi.data.accelY, bmi.data.accelZ) * 180.0f / PI;
    pitchSum += atan2(-bmi.data.accelX, sqrt(bmi.data.accelY * bmi.data.accelY + bmi.data.accelZ * bmi.data.accelZ)) * 180.0f / PI;
    delay(10);
  }
  rollZero = rollSum / 100.0f;
  pitchZero = pitchSum / 100.0f;
}
void setup() {
  Serial.begin(115200); pinMode(STOP_BUTTON, INPUT_PULLUP); Wire.begin(21, 22);
  while (bmi.beginI2C() != BMI2_OK) delay(1000);
  WiFi.mode(WIFI_STA); esp_now_init();
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, vehicleMac, 6); peer.channel = 0; esp_now_add_peer(&peer);
  calibrateNeutral();
}
void loop() {
  bmi.getSensorData();
  const float roll = atan2(bmi.data.accelY, bmi.data.accelZ) * 180.0f / PI - rollZero;
  const float pitch = atan2(-bmi.data.accelX, sqrt(bmi.data.accelY * bmi.data.accelY + bmi.data.accelZ * bmi.data.accelZ)) * 180.0f / PI - pitchZero;
  ControlPacket packet{angleToCommand(pitch), angleToCommand(roll), !digitalRead(STOP_BUTTON), sequenceNumber++};
  esp_now_send(vehicleMac, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
  delay(30);
}
