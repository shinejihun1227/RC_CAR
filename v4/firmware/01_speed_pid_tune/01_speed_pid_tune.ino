// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdint.h>

// 시리얼 명령: f(전진), b(후진), s(정지), +(목표 RPM 증가), -(목표 RPM 감소)
// 좌우 N20 엔코더 모터 2개만 PID로 제어하고, 두 수동 바퀴에는 신호를 연결하지 않는다.
// PULSES_PER_WHEEL_REV와 PID 계수는 사용하는 N20 엔코더에 맞춰 조정한다.

const int IN1[2] = {18, 23};
const int IN2[2] = {19, 25};
const int PWM[2] = {13, 14};
const int ENC[2] = {34, 35};
const float PULSES_PER_WHEEL_REV = 1.0f; // 실측 후 수정
const unsigned long PID_MS = 100;

float trim[2] = {1.00f, 1.00f};
float kp = 1.8f, ki = 0.40f, kd = 0.0f;

volatile int32_t pulses[2] = {0, 0};
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;

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

int32_t previous[2] = {0, 0};
float integral[2] = {0, 0};
float previousError[2] = {0, 0};
float targetRpm = 50.0f;
float measuredRpm[2] = {0, 0};
int direction = 0;
unsigned long lastPid = 0;

void setMotor(int i, int value) {
  digitalWrite(IN1[i], value > 0 ? HIGH : LOW);
  digitalWrite(IN2[i], value < 0 ? HIGH : LOW);
  analogWrite(PWM[i], constrain(abs(value), 0, 255));
}

void stopAll(void) {
  for (int i = 0; i < 2; ++i) {
    setMotor(i, 0);
    integral[i] = 0;
    previousError[i] = 0;
  }
}

void updatePid(void) {
  const unsigned long nowMs = millis();
  if (nowMs - lastPid < PID_MS)
    return;
  const float dt = (nowMs - lastPid) / 1000.0f;
  lastPid = nowMs;

  int32_t count[2];
  portENTER_CRITICAL(&pulseMux);
  for (int i = 0; i < 2; ++i)
    count[i] = pulses[i];
  portEXIT_CRITICAL(&pulseMux);

  for (int i = 0; i < 2; ++i) {
    measuredRpm[i] = (count[i] - previous[i]) * 60.0f /
                     (PULSES_PER_WHEEL_REV * dt);
    previous[i] = count[i];
    if (direction == 0)
      continue;

    const float error = targetRpm - measuredRpm[i];
    integral[i] = constrain(integral[i] + error * dt, -80.0f, 80.0f);
    const float derivative = (error - previousError[i]) / dt;
    const int pwm = constrain((kp * error + ki * integral[i] +
                               kd * derivative) * trim[i], 0, 255);
    setMotor(i, direction * pwm);
    previousError[i] = error;
  }
  Serial.printf("RPM L %.1f R %.1f, target %.1f\n",
                measuredRpm[0], measuredRpm[1], targetRpm);
}

void setup(void) {
  Serial.begin(115200);
  for (int i = 0; i < 2; ++i) {
    pinMode(IN1[i], OUTPUT);
    pinMode(IN2[i], OUTPUT);
    pinMode(PWM[i], OUTPUT);
    pinMode(ENC[i], INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(ENC[0]), encLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC[1]), encRight, RISING);
  stopAll();
  lastPid = millis();
}

void loop(void) {
  if (Serial.available()) {
    const char c = Serial.read();
    if (c == 'f')
      direction = 1;
    if (c == 'b')
      direction = -1;
    if (c == 's')
      direction = 0;
    if (c == '+')
      targetRpm += 5;
    if (c == '-')
      targetRpm = max(5.0f, targetRpm - 5);
  }

  if (direction == 0)
    stopAll();
  else
    updatePid();
}
