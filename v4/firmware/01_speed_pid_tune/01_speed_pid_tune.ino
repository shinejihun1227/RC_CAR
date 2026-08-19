// 시리얼 명령: f(전진), b(후진), s(정지), +(목표 RPM 증가), -(목표 RPM 감소)
// PULSES_PER_WHEEL_REV와 PID 계수는 사용하는 N20 엔코더 모터에 맞춰 조정한다.

constexpr int IN1[4] = {18, 23, 26, 32};
constexpr int IN2[4] = {19, 25, 27, 33};
constexpr int PWM[4] = {13, 14, 16, 17};
constexpr int ENC[4] = {34, 35, 36, 39};
constexpr float PULSES_PER_WHEEL_REV = 1.0f;  // 실측 후 수정
constexpr unsigned long PID_MS = 100;
float trim[4] = {1.00f, 1.00f, 1.00f, 1.00f};
float kp = 1.8f, ki = 0.40f, kd = 0.0f;

volatile int32_t pulses[4] = {0, 0, 0, 0};
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR enc0() { portENTER_CRITICAL_ISR(&pulseMux); pulses[0]++; portEXIT_CRITICAL_ISR(&pulseMux); }
void IRAM_ATTR enc1() { portENTER_CRITICAL_ISR(&pulseMux); pulses[1]++; portEXIT_CRITICAL_ISR(&pulseMux); }
void IRAM_ATTR enc2() { portENTER_CRITICAL_ISR(&pulseMux); pulses[2]++; portEXIT_CRITICAL_ISR(&pulseMux); }
void IRAM_ATTR enc3() { portENTER_CRITICAL_ISR(&pulseMux); pulses[3]++; portEXIT_CRITICAL_ISR(&pulseMux); }

int32_t previous[4] = {0, 0, 0, 0};
float integral[4] = {0, 0, 0, 0}, previousError[4] = {0, 0, 0, 0};
float targetRpm = 50.0f, measuredRpm[4] = {0, 0, 0, 0};
int direction = 0;
unsigned long lastPid = 0;

void setMotor(int i, int value) {
  digitalWrite(IN1[i], value > 0 ? HIGH : LOW);
  digitalWrite(IN2[i], value < 0 ? HIGH : LOW);
  analogWrite(PWM[i], constrain(abs(value), 0, 255));
}
void stopAll() { for (int i = 0; i < 4; ++i) { setMotor(i, 0); integral[i] = 0; previousError[i] = 0; } }
void updatePid() {
  const unsigned long nowMs = millis();
  if (nowMs - lastPid < PID_MS) return;
  const float dt = (nowMs - lastPid) / 1000.0f;
  lastPid = nowMs;
  int32_t count[4];
  portENTER_CRITICAL(&pulseMux); for (int i = 0; i < 4; ++i) count[i] = pulses[i]; portEXIT_CRITICAL(&pulseMux);
  for (int i = 0; i < 4; ++i) {
    measuredRpm[i] = (count[i] - previous[i]) * 60.0f / (PULSES_PER_WHEEL_REV * dt);
    previous[i] = count[i];
    if (direction == 0) continue;
    const float error = targetRpm - measuredRpm[i];
    integral[i] = constrain(integral[i] + error * dt, -80.0f, 80.0f);
    const float derivative = (error - previousError[i]) / dt;
    const int pwm = constrain((kp * error + ki * integral[i] + kd * derivative) * trim[i], 0, 255);
    setMotor(i, direction * pwm);
    previousError[i] = error;
  }
  Serial.printf("RPM FL %.1f FR %.1f RL %.1f RR %.1f, target %.1f\n", measuredRpm[0], measuredRpm[1], measuredRpm[2], measuredRpm[3], targetRpm);
}
void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; ++i) { pinMode(IN1[i], OUTPUT); pinMode(IN2[i], OUTPUT); pinMode(PWM[i], OUTPUT); pinMode(ENC[i], INPUT); }
  attachInterrupt(digitalPinToInterrupt(ENC[0]), enc0, RISING); attachInterrupt(digitalPinToInterrupt(ENC[1]), enc1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC[2]), enc2, RISING); attachInterrupt(digitalPinToInterrupt(ENC[3]), enc3, RISING);
  stopAll(); lastPid = millis();
}
void loop() {
  if (Serial.available()) { const char c = Serial.read(); if (c == 'f') direction = 1; if (c == 'b') direction = -1; if (c == 's') direction = 0; if (c == '+') targetRpm += 5; if (c == '-') targetRpm = max(5.0f, targetRpm - 5); }
  if (direction == 0) stopAll(); else updatePid();
}
