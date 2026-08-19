// 필요 라이브러리: Adafruit BMI270 (또는 사용하는 BMI270 모듈의 Arduino 라이브러리)
// 이 파일은 v2의 ControlPacket과 ESP-NOW 송신부를 재사용한다.
#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_BMI270.h>

constexpr int STOP_BUTTON = 4;
constexpr float DEAD_ZONE_DEG = 8.0f, FULL_SCALE_DEG = 35.0f;
uint8_t vehicleMac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
Adafruit_BMI270 bmi;
struct ControlPacket { int16_t throttle, steering; bool emergencyStop; uint32_t sequence; };
float pitchZero = 0, rollZero = 0; uint32_t sequenceNumber = 0;

int16_t angleToCommand(float degree) {
  if (abs(degree) < DEAD_ZONE_DEG) return 0;
  return constrain((int)((degree / FULL_SCALE_DEG) * 255), -255, 255);
}
void setup() {
  Serial.begin(115200); pinMode(STOP_BUTTON, INPUT_PULLUP); Wire.begin(21, 22);
  if (!bmi.begin_I2C()) while (true) delay(1000);
  WiFi.mode(WIFI_STA); esp_now_init();
  esp_now_peer_info_t peer{}; memcpy(peer.peer_addr, vehicleMac, 6); peer.channel = 0; esp_now_add_peer(&peer);
  // 실제 프로젝트에서는 여기서 1초간 평균을 내 중립 Roll/Pitch를 저장한다.
}
void loop() {
  sensors_event_t accel, gyro, temp; bmi.getEvent(&accel, &gyro, &temp);
  const float roll = atan2(accel.acceleration.y, accel.acceleration.z) * 180.0f / PI - rollZero;
  const float pitch = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180.0f / PI - pitchZero;
  ControlPacket packet{angleToCommand(pitch), angleToCommand(roll), !digitalRead(STOP_BUTTON), sequenceNumber++};
  esp_now_send(vehicleMac, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
  delay(30);
}
