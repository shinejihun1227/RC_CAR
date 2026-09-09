// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>

// RC_CAR v0.2 - 시리얼 명령 기반 기본 주행
// f: 전진, b: 후진, l: 좌회전, r: 우회전, s: 정지

const int STBY = 25;
const int AIN1 = 26;
const int AIN2 = 27;
const int PWMA = 14;
const int BIN1 = 32;
const int BIN2 = 33;
const int PWMB = 13;
const int DRIVE_SPEED = 180;

void setMotor(int in1, int in2, int pwmPin, int speedValue) {
  const int pwm = constrain(abs(speedValue), 0, 255);
  digitalWrite(in1, speedValue > 0 ? HIGH : LOW);
  digitalWrite(in2, speedValue < 0 ? HIGH : LOW);
  analogWrite(pwmPin, pwm);
}

void drive(int leftSpeed, int rightSpeed) {
  setMotor(AIN1, AIN2, PWMA, leftSpeed);
  setMotor(BIN1, BIN2, PWMB, rightSpeed);
}

void setup(void) {
  Serial.begin(115200);
  pinMode(STBY, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  drive(0, 0);
  digitalWrite(STBY, HIGH);
  Serial.println("RC_CAR ready: f/b/l/r/s");
}

void loop(void) {
  if (!Serial.available())
    return;

  switch (tolower(Serial.read())) {
  case 'f':
    drive(DRIVE_SPEED, DRIVE_SPEED);
    break;
  case 'b':
    drive(-DRIVE_SPEED, -DRIVE_SPEED);
    break;
  case 'l':
    drive(-DRIVE_SPEED, DRIVE_SPEED);
    break;
  case 'r':
    drive(DRIVE_SPEED, -DRIVE_SPEED);
    break;
  case 's':
    drive(0, 0);
    break;
  default:
    return;
  }
}
