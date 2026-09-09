// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 필요 라이브러리: Pololu VL53L1X
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <VL53L1X.h>

const int STBY = 25, AIN1 = 26, AIN2 = 27, PWMA = 14;
const int BIN1 = 32, BIN2 = 33, PWMB = 13;
const uint16_t STOP_MM = 200;
// 라이브러리가 제공하는 거리센서 객체. tof.으로 센서 기능을 호출한다.
VL53L1X tof;
// 송신기와 수신기의 필드 순서·자료형을 동일하게 유지한다.
typedef struct {
  int16_t throttle;
  int16_t steering;
  bool emergencyStop;
  uint32_t sequence;
} ControlPacket;
ControlPacket latest = {0};
unsigned long lastReceiveMs = 0;
portMUX_TYPE dataMux = portMUX_INITIALIZER_UNLOCKED;
void motor(int a, int b, int pwmPin, int value) {
  digitalWrite(a, value > 0 ? HIGH : LOW);
  digitalWrite(b, value < 0 ? HIGH : LOW);
  analogWrite(pwmPin, constrain(abs(value), 0, 255));
}
void drive(int left, int right) {
  motor(AIN1, AIN2, PWMA, left);
  motor(BIN1, BIN2, PWMB, right);
}
// 새 측정이 준비됐을 때만 읽어 정지 명령·통신 시간 확인을 계속한다.
const unsigned long SENSOR_MAX_AGE_MS = 250;
uint16_t distanceMm = 0;
bool distanceValid = false;
unsigned long distanceReadMs = 0;

void updateDistance(void) {
  if (!tof.dataReady())
    return;
  distanceMm = tof.read(false);
  distanceValid = !tof.timeoutOccurred() && tof.last_status == 0 &&
                  tof.ranging_data.range_status == VL53L1X::RangeValid;
  distanceReadMs = millis();
}

bool obstacleTooClose(void) {
  return !distanceValid || millis() - distanceReadMs > SENSOR_MAX_AGE_MS || distanceMm <= STOP_MM;
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onReceive(const esp_now_recv_info_t *source, const uint8_t *data, int len) {
#else
void onReceive(const uint8_t *source, const uint8_t *data, int len) {
#endif
  (void)source; // 이번 실습에서는 발신자 정보 대신 명령 데이터만 사용한다.
  if (len != sizeof(ControlPacket))
    return;
  portENTER_CRITICAL(&dataMux);
  memcpy(&latest, data, sizeof(latest));
  lastReceiveMs = millis();
  portEXIT_CRITICAL(&dataMux);
}
void setup(void) {
  pinMode(STBY, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  drive(0, 0);
  digitalWrite(STBY, HIGH);
  Wire.begin(21, 22);
  tof.setTimeout(500);
  if (!tof.init())
    while (true) {
      drive(0, 0);
      delay(1000);
    }
  tof.setDistanceMode(VL53L1X::Long);
  tof.startContinuous(50);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);
}
void loop(void) {
  updateDistance();
  ControlPacket packet;
  unsigned long receivedAt;
  portENTER_CRITICAL(&dataMux);
  packet = latest;
  receivedAt = lastReceiveMs;
  portEXIT_CRITICAL(&dataMux);
  if (millis() - receivedAt > 300 || packet.emergencyStop) {
    drive(0, 0);
    return;
  }
  if (packet.throttle > 0 && obstacleTooClose()) {
    drive(0, 0);
    return;
  }
  const int left = constrain(packet.throttle + packet.steering, -255, 255);
  const int right = constrain(packet.throttle - packet.steering, -255, 255);
  drive(left, right);
}
