// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 필요 라이브러리: Pololu VL53L1X
// PULSES_PER_WHEEL_REV와 PID 계수는 사용하는 엔코더 N20에 맞춰 실측 후 수정한다.
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <VL53L1X.h>

const int IN1[4] = {18, 23, 26, 32};
const int IN2[4] = {19, 25, 27, 33};
const int PWM[4] = {13, 14, 16, 17};
const int ENC[4] = {34, 35, 36, 39};
const uint16_t STOP_MM = 200;
const float MAX_RPM = 100.0f;
const float PULSES_PER_WHEEL_REV = 1.0f;
const unsigned long PID_MS = 100;
float trim[4] = {1.00f, 1.00f, 1.00f, 1.00f};
float kp = 1.8f, ki = 0.40f, kd = 0.0f;

// 송신기와 수신기의 필드 순서·자료형을 동일하게 유지한다.
typedef struct {
  int16_t throttle;
  int16_t steering;
  bool emergencyStop;
  uint32_t sequence;
} ControlPacket;
ControlPacket latest = {0};
portMUX_TYPE dataMux = portMUX_INITIALIZER_UNLOCKED;
volatile unsigned long lastReceiveMs = 0;
volatile int32_t pulses[4] = {0, 0, 0, 0};
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;
// 라이브러리가 제공하는 거리센서 객체. tof.으로 센서 기능을 호출한다.
VL53L1X tof;

void IRAM_ATTR enc0(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[0]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}
void IRAM_ATTR enc1(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[1]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}
void IRAM_ATTR enc2(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[2]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}
void IRAM_ATTR enc3(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[3]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

void setMotor(int i, int value) {
  digitalWrite(IN1[i], value > 0 ? HIGH : LOW);
  digitalWrite(IN2[i], value < 0 ? HIGH : LOW);
  analogWrite(PWM[i], constrain(abs(value), 0, 255));
}
float targetRpm[4] = {0, 0, 0, 0};
int direction[4] = {0, 0, 0, 0};
int32_t previous[4] = {0, 0, 0, 0};
float integral[4] = {0, 0, 0, 0}, previousError[4] = {0, 0, 0, 0};
unsigned long lastPidMs = 0;

void stopAll(void) {
  for (int i = 0; i < 4; ++i) {
    setMotor(i, 0);
    targetRpm[i] = 0;
    direction[i] = 0;
    integral[i] = 0;
    previousError[i] = 0;
  }
}
void setSideTargets(int left, int right) {
  const int commands[4] = {left, right, left, right};
  for (int i = 0; i < 4; ++i) {
    direction[i] = commands[i] > 0 ? 1 : commands[i] < 0 ? -1 : 0;
    targetRpm[i] = abs(commands[i]) * MAX_RPM / 255.0f;
  }
}
void updatePid(void) {
  const unsigned long now = millis();
  if (now - lastPidMs < PID_MS)
    return;
  const float dt = (now - lastPidMs) / 1000.0f;
  lastPidMs = now;
  int32_t count[4];
  portENTER_CRITICAL(&pulseMux);
  for (int i = 0; i < 4; ++i)
    count[i] = pulses[i];
  portEXIT_CRITICAL(&pulseMux);
  for (int i = 0; i < 4; ++i) {
    const float rpm = (count[i] - previous[i]) * 60.0f / (PULSES_PER_WHEEL_REV * dt);
    previous[i] = count[i];
    if (direction[i] == 0) {
      setMotor(i, 0);
      integral[i] = 0;
      previousError[i] = 0;
      continue;
    }
    const float error = targetRpm[i] - rpm;
    integral[i] = constrain(integral[i] + error * dt, -80.0f, 80.0f);
    const float derivative = (error - previousError[i]) / dt;
    const int pwm = constrain((kp * error + ki * integral[i] + kd * derivative) * trim[i], 0, 255);
    setMotor(i, direction[i] * pwm);
    previousError[i] = error;
  }
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
  for (int i = 0; i < 4; ++i) {
    pinMode(IN1[i], OUTPUT);
    pinMode(IN2[i], OUTPUT);
    pinMode(PWM[i], OUTPUT);
    pinMode(ENC[i], INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(ENC[0]), enc0, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC[1]), enc1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC[2]), enc2, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC[3]), enc3, RISING);
  stopAll();
  lastPidMs = millis();
  Wire.begin(21, 22);
  tof.setTimeout(500);
  if (!tof.init())
    while (true) {
      stopAll();
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
    stopAll();
    return;
  }
  if (packet.throttle > 0 && obstacleTooClose()) {
    stopAll();
    return;
  }
  setSideTargets(constrain(packet.throttle + packet.steering, -255, 255),
                 constrain(packet.throttle - packet.steering, -255, 255));
  updatePid();
}
