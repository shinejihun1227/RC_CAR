// Ready: Mega 2560 + L298N + two DC motors + front VL53L0X.
// Repeat left forward, left reverse, right forward, right reverse.
// At 300 mm or less, stop BOTH motors. Resume the same step above 300 mm.
// Keep the wheels off the floor during this practice.
#include <Wire.h>
#include <VL53L0X.h>

const byte ENA = 5, IN1 = 22, IN2 = 23;
const byte ENB = 6, IN3 = 24, IN4 = 25;
const bool LEFT_INVERT = false;   // Set true if the left wheel runs backward.
const bool RIGHT_INVERT = false;  // Set true if the right wheel runs backward.
const byte TEST_PWM = 200;        // Use the PWM value confirmed for your motor.

const uint16_t STOP_DISTANCE_MM = 300;
const unsigned long RANGE_INTERVAL_MS = 75;
const unsigned long RETRY_INTERVAL_MS = 1000;

// A short stop between directions reduces abrupt reversing.
const int8_t LEFT_DIR[8]  = { 1, 0, -1, 0,  0, 0,  0, 0 };
const int8_t RIGHT_DIR[8] = { 0, 0,  0, 0,  1, 0, -1, 0 };
const uint16_t STEP_MS[8] = { 2000, 400, 2000, 400,
                              2000, 400, 2000, 400 };

VL53L0X sensor;
bool sensorReady = false;
bool distanceValid = false;
uint16_t distanceMm = 0;
unsigned long lastInitAttemptMs = 0;
unsigned long lastRangeMs = 0;
unsigned long lastLoopMs = 0;
unsigned long lastReportMs = 0;
unsigned long stepElapsedMs = 0;
byte stepIndex = 0;
int8_t appliedLeft = 2, appliedRight = 2; // Force the first motor update.
bool lastBlocked = true;
byte lastPrintedStep = 255;

void writeMotor(byte en, byte inA, byte inB, int8_t direction, bool invert) {
  if (invert) direction = -direction;
  analogWrite(en, 0);
  digitalWrite(inA, direction > 0 ? HIGH : LOW);
  digitalWrite(inB, direction < 0 ? HIGH : LOW);
  analogWrite(en, direction == 0 ? 0 : TEST_PWM);
}

void drive(int8_t left, int8_t right) {
  if (left == appliedLeft && right == appliedRight) return;
  writeMotor(ENA, IN1, IN2, left, LEFT_INVERT);
  writeMotor(ENB, IN3, IN4, right, RIGHT_INVERT);
  appliedLeft = left;
  appliedRight = right;
}

void startSensor() {
  lastInitAttemptMs = millis();
  distanceValid = false;
  drive(0, 0);
  sensorReady = sensor.init();
  if (!sensorReady) {
    Serial.println(F("VL53L0X missing: both motors stopped"));
    return;
  }
  sensor.startContinuous(RANGE_INTERVAL_MS);
  lastRangeMs = millis();
  Serial.println(F("VL53L0X ready; waiting for a valid distance"));
}

void updateDistance() {
  if (!sensorReady) {
    if (millis() - lastInitAttemptMs >= RETRY_INTERVAL_MS) startSensor();
    return;
  }
  if (millis() - lastRangeMs < RANGE_INTERVAL_MS) return;
  lastRangeMs = millis();
  uint16_t mm = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred()) {
    sensorReady = false; // Retry initialization; keep the motors stopped.
    distanceValid = false;
    return;
  }
  if (mm == 0 || mm == 65535) {
    distanceValid = false;
    return;
  }
  distanceMm = mm;
  distanceValid = true;
}

void printStep() {
  Serial.print(F("step="));
  switch (stepIndex) {
    case 0: Serial.println(F("LEFT_FORWARD")); break;
    case 1: Serial.println(F("STOP")); break;
    case 2: Serial.println(F("LEFT_REVERSE")); break;
    case 3: Serial.println(F("STOP")); break;
    case 4: Serial.println(F("RIGHT_FORWARD")); break;
    case 5: Serial.println(F("STOP")); break;
    case 6: Serial.println(F("RIGHT_REVERSE")); break;
    default: Serial.println(F("STOP")); break;
  }
}

void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  drive(0, 0);
  Serial.begin(115200);
  Wire.begin(); // Mega: SDA=D20, SCL=D21.
  sensor.setTimeout(100);
  startSensor();
  lastLoopMs = millis();
}

void loop() {
  updateDistance();
  unsigned long now = millis();
  unsigned long elapsed = now - lastLoopMs;
  lastLoopMs = now;

  bool blocked = !sensorReady || !distanceValid || distanceMm <= STOP_DISTANCE_MM;
  if (blocked) {
    drive(0, 0);
  } else {
    stepElapsedMs += elapsed; // Time does not advance while blocked.
    if (stepElapsedMs >= STEP_MS[stepIndex]) {
      stepElapsedMs = 0;
      stepIndex = (stepIndex + 1) % 8;
    }
    drive(LEFT_DIR[stepIndex], RIGHT_DIR[stepIndex]);
  }

  if (blocked != lastBlocked || stepIndex != lastPrintedStep) {
    if (blocked) Serial.println(F("STOP: obstacle <= 30 cm or sensor error"));
    else printStep();
    lastBlocked = blocked;
    lastPrintedStep = stepIndex;
  }
  // Arduino IDE Serial Monitor: 115200 baud. Print a live reading every 0.2 s.
  if (now - lastReportMs >= 200) {
    lastReportMs = now;
    Serial.print(F("Distance: "));
    if (distanceValid) {
      Serial.print(distanceMm / 10);
      Serial.print('.');
      Serial.print(distanceMm % 10);
      Serial.print(F(" cm"));
    } else {
      Serial.print(F("SENSOR ERROR"));
    }
    Serial.print(F(" | Motor: "));
    Serial.println(blocked ? F("STOP") : F("RUN"));
  }
}

