// RC_CAR v0.2 - 시리얼 명령 기반 기본 주행
// f: 전진, b: 후진, l: 좌회전, r: 우회전, s: 정지

constexpr int STBY = 25;
constexpr int AIN1 = 26;
constexpr int AIN2 = 27;
constexpr int PWMA = 14;
constexpr int BIN1 = 32;
constexpr int BIN2 = 33;
constexpr int PWMB = 13;
constexpr int DRIVE_SPEED = 180;

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

void setup() {
  Serial.begin(115200);
  for (int pin : {STBY, AIN1, AIN2, PWMA, BIN1, BIN2, PWMB}) {
    pinMode(pin, OUTPUT);
  }
  drive(0, 0);
  digitalWrite(STBY, HIGH);
  Serial.println("RC_CAR ready: f/b/l/r/s");
}

void loop() {
  if (!Serial.available()) return;

  switch (tolower(Serial.read())) {
    case 'f': drive(DRIVE_SPEED, DRIVE_SPEED); break;
    case 'b': drive(-DRIVE_SPEED, -DRIVE_SPEED); break;
    case 'l': drive(-DRIVE_SPEED, DRIVE_SPEED); break;
    case 'r': drive(DRIVE_SPEED, -DRIVE_SPEED); break;
    case 's': drive(0, 0); break;
    default: return;
  }
}
