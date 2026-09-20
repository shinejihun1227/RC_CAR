// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// 필요 라이브러리: Pololu VL53L1X
// 좌우 N20 엔코더 모터 2개만 PID로 제어한다. 뒤쪽 두 바퀴는 수동 바퀴다.
// PULSES_PER_WHEEL_REV와 PID 계수는 사용하는 N20 엔코더에 맞춰 실측한다.
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <VL53L1X.h>

const int IN1[2] = {18, 23};
const int IN2[2] = {19, 25};
const int PWM[2] = {13, 14};
const int ENC[2] = {34, 35};
const uint16_t STOP_MM = 200;
const float MAX_RPM = 100.0f;
const float PULSES_PER_WHEEL_REV = 1.0f;
const unsigned long PID_MS = 100;
float trim[2] = {1.00f, 1.00f};
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
volatile int32_t pulses[2] = {0, 0};
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;
VL53L1X tof;

void IRAM_ATTR encLeft(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[0]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

void IRAM_ATTR encRight(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[1]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

void setMotor(int i, int value) {
  digitalWrite(IN1[i], value > 0 ? HIGH : LOW);
  digitalWrite(IN2[i], value < 0 ? HIGH : LOW);
  analogWrite(PWM[i], constrain(abs(value), 0, 255));
}

float targetRpm[2] = {0, 0};
int direction[2] = {0, 0};
int32_t previous[2] = {0, 0};
float integral[2] = {0, 0};
float previousError[2] = {0, 0};
unsigned long lastPidMs = 0;

void stopAll(void) {
  for (int i = 0; i < 2; ++i) {
    setMotor(i, 0);
    targetRpm[i] = 0;
    direction[i] = 0;
    integral[i] = 0;
    previousError[i] = 0;
  }
}

void setSideTargets(int left, int right) {
  const int commands[2] = {left, right};
  for (int i = 0; i < 2; ++i) {
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

  int32_t count[2];
  portENTER_CRITICAL(&pulseMux);
  for (int i = 0; i < 2; ++i)
    count[i] = pulses[i];
  portEXIT_CRITICAL(&pulseMux);

  for (int i = 0; i < 2; ++i) {
    const float rpm = (count[i] - previous[i]) * 60.0f /
                      (PULSES_PER_WHEEL_REV * dt);
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
    const int pwm = constrain((kp * error + ki * integral[i] +
                               kd * derivative) * trim[i], 0, 255);
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
  return !distanceValid || millis() - distanceReadMs > SENSOR_MAX_AGE_MS ||
         distanceMm <= STOP_MM;
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onReceive(const esp_now_recv_info_t *source, const uint8_t *data, int len) {
#else
void onReceive(const uint8_t *source, const uint8_t *data, int len) {
#endif
  (void)source;
  if (len != sizeof(ControlPacket))
    return;
  portENTER_CRITICAL(&dataMux);
  memcpy(&latest, data, sizeof(latest));
  lastReceiveMs = millis();
  portEXIT_CRITICAL(&dataMux);
}

void setup(void) {
  for (int i = 0; i < 2; ++i) {
    pinMode(IN1[i], OUTPUT);
    pinMode(IN2[i], OUTPUT);
    pinMode(PWM[i], OUTPUT);
    pinMode(ENC[i], INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(ENC[0]), encLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC[1]), encRight, RISING);
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
