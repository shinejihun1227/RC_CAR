// Mega radio skid steering with a front VL53L0X obstacle stop.
// When blocked, only reverse wheel motion is allowed. A failed sensor also blocks forward.
#include <SPI.h>
#include <Wire.h>
#include <RF24.h>
#include <VL53L0X.h>
const byte ENA = 5, IN1 = 22, IN2 = 23, IN3 = 24, IN4 = 25, ENB = 6;
const bool LEFT_INVERT = false;  // Match the result of now_01_motor_mega.
const bool RIGHT_INVERT = false; // Change to true if right wheel runs backward.
const byte MAX_PWM = 200; // Replace with the value confirmed in v0 on your hardware.
const unsigned long RADIO_TIMEOUT_MS = 250;
const unsigned long RANGE_INTERVAL_MS = 75;
const uint16_t STOP_DISTANCE_MM = 300;   // 30 cm or closer: block forward.
const uint16_t RESUME_DISTANCE_MM = 400; // At least 40 cm: allow forward again.
const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
struct Packet { byte magic; int8_t throttle; int8_t turn; byte sequence; };
static_assert(sizeof(Packet) == 4, "Packet layout must match Uno");
RF24 radio(7, 8);
VL53L0X sensor;
unsigned long lastPacketMs = 0;
unsigned long lastDriveMs = 0;
unsigned long lastRangeMs = 0;
unsigned long lastReportMs = 0;
bool linked = false;
bool sensorReady = false;
bool forwardBlocked = true;
uint16_t lastDistanceMm = 0;
int8_t throttle = 0, turn = 0;

void motor(byte en, byte a, byte b, int value, bool invert) {
  value = constrain(value, -100, 100);
  if (invert) value = -value;
  analogWrite(en, 0);
  digitalWrite(a, value > 0 ? HIGH : LOW);
  digitalWrite(b, value < 0 ? HIGH : LOW);
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
    lastDistanceMm = 0;
    forwardBlocked = true;
    return;
  }
  lastDistanceMm = mm;
  if (mm <= STOP_DISTANCE_MM) forwardBlocked = true;
  else if (mm >= RESUME_DISTANCE_MM) forwardBlocked = false;
}
void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  drive(0, 0);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  Wire.begin(); // Mega SDA=D20, SCL=D21.
  sensor.setTimeout(100);
  sensorReady = sensor.init();
  if (sensorReady) {
    sensor.startContinuous(RANGE_INTERVAL_MS);
    Serial.println(F("VL53L0X ready"));
  } else {
    Serial.println(F("VL53L0X missing: forward blocked, reverse allowed"));
  }
  pinMode(53, OUTPUT); digitalWrite(53, HIGH);
  if (!radio.begin()) {
    while (true) { drive(0, 0); digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(100); }
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setPayloadSize(sizeof(Packet));
  radio.openReadingPipe(1, ADDRESS);
  radio.startListening();
}
void loop() {
  while (radio.available()) {
    Packet p;
    radio.read(&p, sizeof(p));
    if (p.magic == 0xA5 && p.throttle >= -100 && p.throttle <= 100 && p.turn >= -100 && p.turn <= 100) {
      throttle = p.throttle;
      turn = p.turn;
      lastPacketMs = millis();
      linked = true;
    }
  }
  updateDistance();
  if (millis() - lastReportMs >= 500) {
    lastReportMs = millis();
    Serial.print(F("distance_mm=")); Serial.print(lastDistanceMm);
    Serial.print(F(" blocked=")); Serial.print(forwardBlocked ? F("YES") : F("NO"));
    Serial.print(F(" radio="));
    Serial.println(linked && millis() - lastPacketMs < RADIO_TIMEOUT_MS ? F("OK") : F("LOST"));
  }
  if (millis() - lastDriveMs < 20) return;
  lastDriveMs = millis();
  bool fresh = linked && millis() - lastPacketMs < RADIO_TIMEOUT_MS;
  digitalWrite(13, fresh ? HIGH : LOW);
  if (!fresh) { drive(0, 0); return; }

  // Positive turn means left: slow/reverse left, speed up right.
  int left = (int)throttle - (int)turn;
  int right = (int)throttle + (int)turn;
  int peak = max(abs(left), abs(right));
  if (peak > 100) { left = left * 100 / peak; right = right * 100 / peak; }
  if (forwardBlocked) {
    // Stop a forward or turn command. During reverse, never drive either wheel forward.
    if (throttle >= 0) { drive(0, 0); return; }
    if (left > 0) left = 0;
    if (right > 0) right = 0;
  }
  drive(left, right);
}
