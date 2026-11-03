// Arduino Mega 2560: nRF24L01+ receiver, VL53L0X front stop, L298N 2WD drive.
// Bench power: 12 V adapter to L298N, 5V-EN ON, ENA/ENB jumpers OFF,
// Mega from USB, Mega GND joined to L298N GND. NEVER join their 5V pins here.
// Confirm 12 V motor rating and driver current before applying motor power.

#include <SPI.h>
#include <Wire.h>
#include <RF24.h>
#include <VL53L0X.h>

#define VEHICLE_ID 1            // Set to the same value in controller_transmitter.
#define RADIO_CHANNEL 76       // Set to the same value in controller_transmitter.

const uint8_t PIN_ENA = 5;
const uint8_t PIN_ENB = 6;
const uint8_t PIN_IN1 = 22;
const uint8_t PIN_IN2 = 23;
const uint8_t PIN_IN3 = 24;
const uint8_t PIN_IN4 = 25;
const uint8_t PIN_RADIO_CE = 7;
const uint8_t PIN_RADIO_CSN = 8;

const bool LEFT_MOTOR_INVERTED = false;   // Adjust after a raised-wheel test.
const bool RIGHT_MOTOR_INVERTED = false;  // Adjust after a raised-wheel test.
const uint8_t MAX_PWM = 140;              // 0..255; conservative first test.
const uint16_t STOP_DISTANCE_MM = 300;   // Example only; measure stopping distance.
const uint16_t RESUME_DISTANCE_MM = 400; // Hysteresis prevents rapid toggling.
const uint32_t RADIO_TIMEOUT_MS = 300;
const uint32_t RANGE_INTERVAL_MS = 75;

struct ControlPacket {
  uint8_t magic;
  uint8_t vehicleId;
  int8_t throttle;   // -100..100
  int8_t steering;   // -100..100
  uint8_t sequence;
};
static_assert(sizeof(ControlPacket) == 5, "Controller and vehicle packet must be 5 bytes");

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);
VL53L0X sensor;
ControlPacket latestCommand = {0xA5, VEHICLE_ID, 0, 0, 0};
bool haveCommand = false;
bool sensorReady = false;
bool forwardBlocked = true;  // Fail closed until a good distance reading arrives.
uint16_t lastDistanceMm = 0;
uint32_t lastCommandMs = 0;
uint32_t lastRangeMs = 0;
uint32_t lastReportMs = 0;

void makeRadioAddress(uint8_t address[6]) {
  address[0] = 'R';
  address[1] = 'C';
  address[2] = '0';
  address[3] = '0' + VEHICLE_ID / 10;
  address[4] = '0' + VEHICLE_ID % 10;
  address[5] = 0;
}

void driveOne(uint8_t en, uint8_t inA, uint8_t inB, int16_t command, bool inverted) {
  if (inverted) command = -command;
  command = constrain(command, -100, 100);

  // Disable the bridge while changing direction.
  analogWrite(en, 0);
  digitalWrite(inA, command > 0 ? HIGH : LOW);
  digitalWrite(inB, command < 0 ? HIGH : LOW);
  analogWrite(en, (uint8_t)(abs(command) * MAX_PWM / 100));
}

void drive(int16_t left, int16_t right) {
  driveOne(PIN_ENA, PIN_IN1, PIN_IN2, left, LEFT_MOTOR_INVERTED);
  driveOne(PIN_ENB, PIN_IN3, PIN_IN4, right, RIGHT_MOTOR_INVERTED);
}

void updateRange() {
  if (!sensorReady) return;
  if ((uint32_t)(millis() - lastRangeMs) < RANGE_INTERVAL_MS) return;
  lastRangeMs = millis();

  uint16_t mm = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred() || mm == 0 || mm == 65535) {
    lastDistanceMm = 0;
    forwardBlocked = true;
    return;
  }

  lastDistanceMm = mm;
  if (mm <= STOP_DISTANCE_MM) forwardBlocked = true;
  if (mm >= RESUME_DISTANCE_MM) forwardBlocked = false;
}

void receiveCommands() {
  while (radio.available()) {
    ControlPacket incoming;
    radio.read(&incoming, sizeof(incoming));
    if (incoming.magic != 0xA5 || incoming.vehicleId != VEHICLE_ID) continue;
    if (incoming.throttle < -100 || incoming.throttle > 100 ||
        incoming.steering < -100 || incoming.steering > 100) continue;
    latestCommand = incoming;
    lastCommandMs = millis();
    haveCommand = true;
  }
}

void applyCommand() {
  bool fresh = haveCommand && (uint32_t)(millis() - lastCommandMs) <= RADIO_TIMEOUT_MS;
  if (!fresh) {
    drive(0, 0);
    return;
  }

  int16_t left = (int16_t)latestCommand.throttle + latestCommand.steering;
  int16_t right = (int16_t)latestCommand.throttle - latestCommand.steering;
  int16_t peak = max(abs(left), abs(right));
  if (peak > 100) {
    left = left * 100 / peak;
    right = right * 100 / peak;
  }

  // Each wheel is blocked from turning forward when an obstacle is near.
  // Reverse remains available to back away.
  if (forwardBlocked) {
    if (left > 0) left = 0;
    if (right > 0) right = 0;
  }
  drive(left, right);
}

void setup() {
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  drive(0, 0);

  Serial.begin(115200);
  Wire.begin();
  sensor.setTimeout(100);
  sensorReady = sensor.init();
  if (sensorReady) {
    sensor.startContinuous(RANGE_INTERVAL_MS);
    Serial.println(F("Distance sensor ready"));
  } else {
    Serial.println(F("Distance sensor missing: forward motion disabled"));
  }

  pinMode(53, OUTPUT);  // Mega hardware SS must remain an output in SPI master mode.
  digitalWrite(53, HIGH);
  if (!radio.begin()) {
    Serial.println(F("nRF24L01 not found. Motors remain stopped."));
    while (true) {
      drive(0, 0);
      delay(1000);
    }
  }
  uint8_t address[6];
  makeRadioAddress(address);
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setPayloadSize(sizeof(ControlPacket));
  radio.openReadingPipe(1, address);
  radio.startListening();
  Serial.println(F("Receiver ready; motor outputs wait for valid radio commands"));
}

void loop() {
  receiveCommands();
  updateRange();
  applyCommand();

  if ((uint32_t)(millis() - lastReportMs) >= 500) {
    lastReportMs = millis();
    Serial.print(F("id=")); Serial.print(VEHICLE_ID);
    Serial.print(F(" seq=")); Serial.print(latestCommand.sequence);
    Serial.print(F(" cmd=")); Serial.print((int)latestCommand.throttle);
    Serial.print(','); Serial.print((int)latestCommand.steering);
    Serial.print(F(" mm=")); Serial.print(lastDistanceMm);
    Serial.print(F(" blocked=")); Serial.print(forwardBlocked);
    Serial.print(F(" link="));
    Serial.println(haveCommand && (uint32_t)(millis() - lastCommandMs) <= RADIO_TIMEOUT_MS ? F("OK") : F("LOST"));
  }
}
