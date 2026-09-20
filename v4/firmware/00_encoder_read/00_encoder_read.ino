// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdint.h>

// 좌우 구동축의 N20 엔코더 A 채널 2개를 읽는 단품 확인 코드.
// 뒤쪽 두 바퀴는 수동 바퀴이므로 엔코더 입력을 연결하지 않는다.
// PULSES_PER_WHEEL_REV는 사용하는 N20 엔코더의 실제 출력축 값으로 바꾼다.

const int ENC_LEFT = 34;
const int ENC_RIGHT = 35;
const float PULSES_PER_WHEEL_REV = 1.0f; // 실측 후 수정
const unsigned long SAMPLE_MS = 500;

volatile int32_t pulses[2] = {0, 0};
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onLeft(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[0]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

void IRAM_ATTR onRight(void) {
  portENTER_CRITICAL_ISR(&pulseMux);
  pulses[1]++;
  portEXIT_CRITICAL_ISR(&pulseMux);
}

unsigned long lastSample = 0;
int32_t previous[2] = {0, 0};

void setup(void) {
  Serial.begin(115200);
  const int encoderPins[2] = {ENC_LEFT, ENC_RIGHT};
  for (int i = 0; i < 2; ++i)
    pinMode(encoderPins[i], INPUT);

  attachInterrupt(digitalPinToInterrupt(ENC_LEFT), onLeft, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_RIGHT), onRight, RISING);
  Serial.println("LEFT,RIGHT encoder pulses and RPM");
}

void loop(void) {
  if (millis() - lastSample < SAMPLE_MS)
    return;
  lastSample = millis();

  int32_t now[2];
  portENTER_CRITICAL(&pulseMux);
  for (int i = 0; i < 2; ++i)
    now[i] = pulses[i];
  portEXIT_CRITICAL(&pulseMux);

  for (int i = 0; i < 2; ++i) {
    const int32_t delta = now[i] - previous[i];
    const float rpm = delta * 60000.0f / (PULSES_PER_WHEEL_REV * SAMPLE_MS);
    Serial.print(i == 0 ? "LEFT " : "RIGHT ");
    Serial.print(delta);
    Serial.print(" pulses / ");
    Serial.print(rpm, 1);
    Serial.print(" RPM");
    if (i == 0)
      Serial.print(" | ");
    previous[i] = now[i];
  }
  Serial.println();
}
