// v0 기본 주행 코드에 전방 거리 안전 정지를 추가한 예제.
// 필요 라이브러리: Pololu VL53L1X
#include <Wire.h>
#include <VL53L1X.h>

constexpr int STBY = 25, AIN1 = 26, AIN2 = 27, PWMA = 14;
constexpr int BIN1 = 32, BIN2 = 33, PWMB = 13, SPEED = 180;
constexpr uint16_t WARNING_MM = 500, STOP_MM = 200;
VL53L1X tof;

void motor(int a, int b, int pwmPin, int value) {
  digitalWrite(a, value > 0 ? HIGH : LOW);
  digitalWrite(b, value < 0 ? HIGH : LOW);
  analogWrite(pwmPin, constrain(abs(value), 0, 255));
}
void drive(int left, int right) { motor(AIN1, AIN2, PWMA, left); motor(BIN1, BIN2, PWMB, right); }
bool obstacleTooClose() {
  const uint16_t mm = tof.read();
  if (tof.timeoutOccurred()) return true;
  if (mm < WARNING_MM) Serial.printf("warning: %u mm\n", mm);
  return mm < STOP_MM;
}
void setup() {
  Serial.begin(115200);
  for (int p : {STBY, AIN1, AIN2, PWMA, BIN1, BIN2, PWMB}) pinMode(p, OUTPUT);
  drive(0, 0); digitalWrite(STBY, HIGH);
  Wire.begin(21, 22); tof.setTimeout(500);
  if (!tof.init()) while (true) { drive(0, 0); delay(1000); }
  tof.setDistanceMode(VL53L1X::Long); tof.startContinuous(50);
}
void loop() {
  if (!Serial.available()) return;
  const char command = tolower(Serial.read());
  if (command == 'f' && obstacleTooClose()) { drive(0, 0); Serial.println("forward blocked"); return; }
  if (command == 'f') drive(SPEED, SPEED);
  if (command == 'b') drive(-SPEED, -SPEED);
  if (command == 'l') drive(-SPEED, SPEED);
  if (command == 'r') drive(SPEED, -SPEED);
  if (command == 's') drive(0, 0);
}
