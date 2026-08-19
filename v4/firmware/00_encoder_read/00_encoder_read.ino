// 엔코더 A 채널 4개를 읽는 단품 확인 코드.
// PULSES_PER_WHEEL_REV는 사용하는 N20 엔코더 모터의 실제 값으로 바꾼다.

constexpr int ENC_FL = 34, ENC_FR = 35, ENC_RL = 36, ENC_RR = 39;
constexpr float PULSES_PER_WHEEL_REV = 1.0f;  // 실측 후 수정
constexpr unsigned long SAMPLE_MS = 500;

volatile int32_t pulses[4] = {0, 0, 0, 0};
portMUX_TYPE pulseMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onFl() { portENTER_CRITICAL_ISR(&pulseMux); pulses[0]++; portEXIT_CRITICAL_ISR(&pulseMux); }
void IRAM_ATTR onFr() { portENTER_CRITICAL_ISR(&pulseMux); pulses[1]++; portEXIT_CRITICAL_ISR(&pulseMux); }
void IRAM_ATTR onRl() { portENTER_CRITICAL_ISR(&pulseMux); pulses[2]++; portEXIT_CRITICAL_ISR(&pulseMux); }
void IRAM_ATTR onRr() { portENTER_CRITICAL_ISR(&pulseMux); pulses[3]++; portEXIT_CRITICAL_ISR(&pulseMux); }

unsigned long lastSample = 0;
int32_t previous[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(115200);
  for (int pin : {ENC_FL, ENC_FR, ENC_RL, ENC_RR}) pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_FL), onFl, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_FR), onFr, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_RL), onRl, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_RR), onRr, RISING);
  Serial.println("FL,FR,RL,RR pulses and RPM");
}

void loop() {
  if (millis() - lastSample < SAMPLE_MS) return;
  lastSample = millis();
  int32_t now[4];
  portENTER_CRITICAL(&pulseMux);
  for (int i = 0; i < 4; ++i) now[i] = pulses[i];
  portEXIT_CRITICAL(&pulseMux);
  for (int i = 0; i < 4; ++i) {
    const int32_t delta = now[i] - previous[i];
    const float rpm = delta * 60000.0f / (PULSES_PER_WHEEL_REV * SAMPLE_MS);
    Serial.print(delta); Serial.print(" pulses / "); Serial.print(rpm, 1); Serial.print(" RPM");
    if (i < 3) Serial.print(" | ");
    previous[i] = now[i];
  }
  Serial.println();
}
