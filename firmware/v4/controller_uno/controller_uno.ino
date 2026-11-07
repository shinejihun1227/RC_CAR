// v4: Uno remote. KCD1 D4 open=joysticks, closed to GND=BMI270 tilt.
// Joystick left VRx=A0 controls throttle; right VRy=A3 controls turn.
// BMI270 uses NiusIMU 0.4.1. Confirm breakout power/I2C levels first.
#include <SPI.h>
#include <Wire.h>
#include <RF24.h>
#include <BMI270.h>

const bool FORWARD_WHEN_A0_HIGH = true; // Set from joystick diagnostic.
const bool LEFT_WHEN_A3_HIGH = false;  // Set from joystick diagnostic.
const int8_t TILT_FORWARD_SIGN = -1;  // Change after checking physical mounting.
const int8_t TILT_LEFT_SIGN = 1;
const int JOYSTICK_DEADZONE = 60;
const float TILT_DEADZONE_G = 0.08f;
const float TILT_FULL_G = 0.55f;
const unsigned long MODE_SETTLE_MS = 150;
const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
struct Packet { byte magic; int8_t throttle; int8_t turn; byte sequence; };
static_assert(sizeof(Packet) == 4, "Packet must match the Mega sketch");

RF24 radio(7, 8); // CE, CSN.
BMI270 imu;
bool imuReady = false;
bool tiltZeroValid = false;
bool joystickCentered = false;
bool tiltMode = false;
bool lastAck = false;
float tiltZeroX = 0, tiltZeroY = 0;
float lastAccelX = 0, lastAccelY = 0;
int8_t throttle = 0, turn = 0;
byte sequence = 0;
unsigned long lastModeChangeMs = 0, lastSendMs = 0, lastReportMs = 0;

int8_t joystickAxis(int raw, bool positiveWhenHigh) {
  int delta = raw - 512;
  if (abs(delta) <= JOYSTICK_DEADZONE) return 0;
  int value = map(abs(delta), JOYSTICK_DEADZONE, 511, 0, 100);
  value = constrain(value, 0, 100);
  return (delta > 0) == positiveWhenHigh ? value : -value;
}

int8_t tiltAxis(float deltaG, int8_t sign) {
  float magnitude = fabs(deltaG);
  if (magnitude <= TILT_DEADZONE_G) return 0;
  float value = (magnitude - TILT_DEADZONE_G) * 100.0f /
                (TILT_FULL_G - TILT_DEADZONE_G);
  if (value > 100.0f) value = 100.0f;
  return (int8_t)((deltaG < 0 ? -value : value) * sign);
}

void updateMode() {
  bool nextMode = digitalRead(4) == LOW;
  if (nextMode == tiltMode) return;
  tiltMode = nextMode;
  lastModeChangeMs = millis();
  throttle = turn = 0; // Send neutral immediately after switching.
  joystickCentered = false;
  tiltZeroValid = false;
}

void readJoysticks() {
  int rawA0 = analogRead(A0);
  int rawA3 = analogRead(A3);
  if (abs(rawA0 - 512) <= JOYSTICK_DEADZONE &&
      abs(rawA3 - 512) <= JOYSTICK_DEADZONE) joystickCentered = true;
  throttle = joystickCentered ? joystickAxis(rawA0, FORWARD_WHEN_A0_HIGH) : 0;
  turn = joystickCentered ? joystickAxis(rawA3, LEFT_WHEN_A3_HIGH) : 0;
}

void readTilt() {
  if (!imuReady || !imu.update()) {
    throttle = turn = 0;
    tiltZeroValid = false;
    return;
  }
  Vec3 accel = imu.accelG();
  lastAccelX = accel.x;
  lastAccelY = accel.y;
  if (!tiltZeroValid) {
    tiltZeroX = accel.x;
    tiltZeroY = accel.y;
    tiltZeroValid = true;
    throttle = turn = 0;
    return;
  }
  throttle = tiltAxis(accel.y - tiltZeroY, TILT_FORWARD_SIGN);
  turn = tiltAxis(accel.x - tiltZeroX, TILT_LEFT_SIGN);
}

void setup() {
  Serial.begin(115200);
  pinMode(4, INPUT_PULLUP); // KCD1 contact to GND; never to 12 V.
  tiltMode = digitalRead(4) == LOW;
  lastModeChangeMs = millis();
  pinMode(10, OUTPUT); digitalWrite(10, HIGH); // Uno hardware SS.

  Wire.begin(); // Uno SDA=A4, SCL=A5.
  imu.setWakeTimeoutMs(100);
  imuReady = imu.beginI2C(Wire, 0x68);
  if (!imuReady) imuReady = imu.beginI2C(Wire, 0x69);
  Serial.print(F("BMI270=")); Serial.println(imuReady ? F("OK") : F("MISSING"));

  if (!radio.begin()) {
    Serial.println(F("Radio not found: check 3.3 V supply and SPI"));
    while (true) delay(1000);
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPayloadSize(sizeof(Packet));
  radio.openWritingPipe(ADDRESS);
  radio.stopListening();
  Serial.println(F("D4 open=JOY, D4-GND=TILT. Center sticks; hold level to enter TILT."));
}

void loop() {
  updateMode();
  if (millis() - lastModeChangeMs < MODE_SETTLE_MS) {
    throttle = turn = 0;
  } else if (tiltMode) {
    readTilt();
  } else {
    readJoysticks();
  }

  if (millis() - lastSendMs >= 50) {
    lastSendMs = millis();
    Packet p = {0xA5, throttle, turn, sequence++};
    lastAck = radio.write(&p, sizeof(p));
  }
  if (millis() - lastReportMs >= 500) {
    lastReportMs = millis();
    Serial.print(F("mode=")); Serial.print(tiltMode ? F("TILT") : F("JOY"));
    Serial.print(F(" imu=")); Serial.print(imuReady ? F("OK") : F("NO"));
    if (tiltMode) {
      Serial.print(F(" ax=")); Serial.print(lastAccelX, 2);
      Serial.print(F(" ay=")); Serial.print(lastAccelY, 2);
    }
    Serial.print(F(" cmd=")); Serial.print((int)throttle);
    Serial.print(','); Serial.print((int)turn);
    Serial.print(F(" ACK=")); Serial.println(lastAck ? F("YES") : F("NO"));
  }
}
