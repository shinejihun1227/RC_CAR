// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>

// RC_CAR v0.1 - TB6612FNG 모터 단품 테스트

const int STBY = 25;
const int AIN1 = 26;
const int AIN2 = 27;
const int PWMA = 14;
const int BIN1 = 32;
const int BIN2 = 33;
const int PWMB = 13;

void setMotor(int in1, int in2, int pwmPin, int speedValue) {
  const int pwm = constrain(abs(speedValue), 0, 255);

  if (speedValue > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (speedValue < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }

  analogWrite(pwmPin, pwm);
}

void stopMotors(void) {
  setMotor(AIN1, AIN2, PWMA, 0);
  setMotor(BIN1, BIN2, PWMB, 0);
}

void setup(void) {
  pinMode(STBY, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  stopMotors();
  digitalWrite(STBY, HIGH);
}

void loop(void) {
  setMotor(AIN1, AIN2, PWMA, 160);
  setMotor(BIN1, BIN2, PWMB, 160);
  delay(1000);

  stopMotors();
  delay(500);

  setMotor(AIN1, AIN2, PWMA, -160);
  setMotor(BIN1, BIN2, PWMB, -160);
  delay(1000);

  stopMotors();
  delay(1500);
}
