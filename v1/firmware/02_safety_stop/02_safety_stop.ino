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
// 새 측정이 준비됐을 때만 읽어 정지 명령·통신 시간 확인을 계속한다.
constexpr unsigned long SENSOR_MAX_AGE_MS = 250;
uint16_t distanceMm = 0;
bool distanceValid = false;
unsigned long distanceReadMs = 0;

void updateDistance() {
  if (!tof.dataReady()) return;
  distanceMm = tof.read(false);
  distanceValid = !tof.timeoutOccurred() && tof.last_status == 0 &&
                  tof.ranging_data.range_status == VL53L1X::RangeValid;
  distanceReadMs = millis();
  if (distanceValid && distanceMm <= WARNING_MM) Serial.printf("warning: %u mm\n", distanceMm);
}

bool obstacleTooClose() {
  return !distanceValid || millis() - distanceReadMs > SENSOR_MAX_AGE_MS ||
         distanceMm <= STOP_MM;
}
char currentCommand = 's';

void setup() {
  Serial.begin(115200);
  for (int p : {STBY, AIN1, AIN2, PWMA, BIN1, BIN2, PWMB}) pinMode(p, OUTPUT);
  drive(0, 0); digitalWrite(STBY, HIGH);
  Wire.begin(21, 22); tof.setTimeout(500);
  if (!tof.init()) while (true) { drive(0, 0); delay(1000); }
  tof.setDistanceMode(VL53L1X::Long); tof.startContinuous(50);
}
void loop() {
  updateDistance();
  while (Serial.available()) {
    const char command = tolower(Serial.read());
    if (command == 'f' || command == 'b' || command == 'l' ||
        command == 'r' || command == 's') currentCommand = command;
  }
  // f를 한 번만 입력해도 주행 중 계속 센서를 감시한다.
  // 장애물로 멈춘 뒤 자동 재출발하지 않고 새 f 명령을 기다린다.
  if (currentCommand == 'f' && obstacleTooClose()) {
    currentCommand = 's';
    Serial.println("forward blocked");
  }
  if (currentCommand == 'f') drive(SPEED, SPEED);
  if (currentCommand == 'b') drive(-SPEED, -SPEED);
  if (currentCommand == 'l') drive(-SPEED, SPEED);
  if (currentCommand == 'r') drive(SPEED, -SPEED);
  if (currentCommand == 's') drive(0, 0);
  delay(1);
}
