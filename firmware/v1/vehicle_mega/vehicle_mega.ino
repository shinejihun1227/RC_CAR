// v1: Mega + L298N + two motors + front VL53L0X. No radio yet.
// Serial monitor (115200, no line ending): w=forward, s=reverse,
// a=left turn, d=right turn, x=stop. Each command expires after 700 ms.
#include <Wire.h>
#include <VL53L0X.h>

const byte ENA = 5, IN1 = 22, IN2 = 23, IN3 = 24, IN4 = 25, ENB = 6;
const bool LEFT_INVERT = false;   // Copy the result from v0.
const bool RIGHT_INVERT = false;  // Copy the result from v0.
const byte MAX_PWM = 200;         // Use only after checking the motor's rated voltage.
const uint16_t STOP_DISTANCE_MM = 300;
const uint16_t RESUME_DISTANCE_MM = 400;
const unsigned long RANGE_INTERVAL_MS = 75;
const unsigned long COMMAND_HOLD_MS = 700;
static_assert(STOP_DISTANCE_MM < RESUME_DISTANCE_MM, "Stop must be nearer than resume");

VL53L0X sensor;
bool sensorReady = false;
bool forwardBlocked = true;
uint16_t distanceMm = 0;
int8_t throttle = 0, turn = 0;
unsigned long lastRangeMs = 0, lastCommandMs = 0, lastReportMs = 0;

void motor(byte en, byte inA, byte inB, int value, bool invert) {
  value = constrain(value, -100, 100);
  if (invert) value = -value;
  analogWrite(en, 0);
  digitalWrite(inA, value > 0 ? HIGH : LOW);
  digitalWrite(inB, value < 0 ? HIGH : LOW);
  analogWrite(en, (byte)(abs(value) * MAX_PWM / 100));
}

void drive(int left, int right) {
  motor(ENA, IN1, IN2, left, LEFT_INVERT);
  motor(ENB, IN3, IN4, right, RIGHT_INVERT);
}

void updateDistance() {
  if (!sensorReady || millis() - lastRangeMs < RANGE_INTERVAL_MS) return;
  lastRangeMs = millis();
  uint16_t mm = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred() || mm == 0 || mm == 65535) {
    distanceMm = 0;
    forwardBlocked = true;
    return;
  }
  distanceMm = mm;
  if (mm <= STOP_DISTANCE_MM) forwardBlocked = true;
  else if (mm >= RESUME_DISTANCE_MM) forwardBlocked = false;
}

void readCommand() {
  while (Serial.available()) {
    char key = Serial.read();
    switch (key) {
      case 'w': throttle = 100; turn = 0; break;
      case 's': throttle = -100; turn = 0; break;
      case 'a': throttle = 0; turn = 100; break;
      case 'd': throttle = 0; turn = -100; break;
      case 'x': throttle = 0; turn = 0; break;
      default: continue; // Ignore CR/LF and other characters.
    }
    lastCommandMs = millis();
  }
}

void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  drive(0, 0);
  Serial.begin(115200);
  Wire.begin(); // Mega SDA=D20, SCL=D21.
  sensor.setTimeout(100);
  sensorReady = sensor.init();
  if (sensorReady) {
    sensor.startContinuous(RANGE_INTERVAL_MS);
    Serial.println(F("VL53L0X ready"));
  } else {
    Serial.println(F("VL53L0X missing: forward blocked"));
  }
  Serial.println(F("Send w/s/a/d/x. Commands expire after 700 ms."));
}

void loop() {
  readCommand();
  updateDistance();
  if (millis() - lastCommandMs >= COMMAND_HOLD_MS) throttle = turn = 0;

  int left = (int)throttle - (int)turn;
  int right = (int)throttle + (int)turn;
  if (forwardBlocked) {
    if (throttle >= 0) left = right = 0;
    else { if (left > 0) left = 0; if (right > 0) right = 0; }
  }
  drive(left, right);

  if (millis() - lastReportMs >= 500) {
    lastReportMs = millis();
    Serial.print(F("distance_mm=")); Serial.print(distanceMm);
    Serial.print(F(" blocked=")); Serial.print(forwardBlocked ? F("YES") : F("NO"));
    Serial.print(F(" command=")); Serial.print((int)throttle);
    Serial.print(','); Serial.println((int)turn);
  }
}
