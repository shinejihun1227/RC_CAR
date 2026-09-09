// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>

const int JOY_X = 34, JOY_Y = 35, STOP_BUTTON = 4;
void setup(void) {
  Serial.begin(115200);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
}
void loop(void) {
  Serial.printf("x=%d y=%d stop=%d\n", analogRead(JOY_X), analogRead(JOY_Y),
                !digitalRead(STOP_BUTTON));
  delay(100);
}
