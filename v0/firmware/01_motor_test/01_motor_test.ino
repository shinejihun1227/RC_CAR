// RC_CAR v0.1 - TB6612FNG 모터 단품 테스트

constexpr int STBY = 25;
constexpr int AIN1 = 26;
constexpr int AIN2 = 27;
constexpr int PWMA = 14;
constexpr int BIN1 = 32;
constexpr int BIN2 = 33;
constexpr int PWMB = 13;

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

void stopMotors() {
  setMotor(AIN1, AIN2, PWMA, 0);
  setMotor(BIN1, BIN2, PWMB, 0);
}

void setup() {
  for (int pin : {STBY, AIN1, AIN2, PWMA, BIN1, BIN2, PWMB}) {
    pinMode(pin, OUTPUT);
  }

  stopMotors();
  digitalWrite(STBY, HIGH);
}

void loop() {
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
