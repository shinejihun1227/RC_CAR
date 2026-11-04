// Uno remote: KCD1 on D4/GND selects joystick (open) or BMI270 tilt (closed).
// nRF24L01+ PA/LNA uses a verified 5 V-input/3.3 V-output socket adapter.
// BMI270 breakout must be safe for Uno's 5 V I2C, or use a level shifter.
// Joystick 2 and the joystick pushbuttons are reserved for later arm control.

#include <SPI.h>
#include <Wire.h>
#include <RF24.h>
#include <BMI270.h> // NiusIMU 0.4.1; SparkFun BMI270 library exceeds Uno RAM.

#define VEHICLE_ID 1
#define RADIO_CHANNEL 76
#ifndef USE_JOYSTICK
#define USE_JOYSTICK 1 // 1=KCD1 selects joystick/tilt; 0=serial commands for bench.
#endif

const uint8_t PIN_RADIO_CE = 7;
const uint8_t PIN_RADIO_CSN = 8;
const uint8_t PIN_MODE_SWITCH = 4;
const uint32_t SEND_INTERVAL_MS = 50;
const uint32_t SERIAL_HOLD_MS = 800;
const int16_t JOYSTICK_DEADZONE = 60;
const float TILT_DEADZONE_G = 0.08f;
const float TILT_FULL_G = 0.55f;
const int8_t TILT_THROTTLE_SIGN = -1; // Change after checking actual mounting.
const int8_t TILT_STEERING_SIGN = 1;

struct ControlPacket {
  uint8_t magic;
  uint8_t vehicleId;
  int8_t throttle;
  int8_t steering;
  uint8_t sequence;
};
static_assert(sizeof(ControlPacket) == 5, "Vehicle and controller packets must match");

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);
BMI270 imu;
bool imuReady = false;
int8_t throttle = 0;
int8_t steering = 0;
uint8_t sequence = 0;
uint8_t lastMode = 255;
bool joystickCentered = false;
bool tiltZeroValid = false;
float tiltZeroX = 0.0f;
float tiltZeroY = 0.0f;
float lastAccelX = 0.0f;
float lastAccelY = 0.0f;
uint32_t lastSerialCommandMs = 0;
uint32_t lastSendMs = 0;
uint32_t lastReportMs = 0;
bool serialCommandActive = false;
bool lastAck = false;

void makeRadioAddress(uint8_t address[6]) {
  address[0] = 'R'; address[1] = 'C'; address[2] = '0';
  address[3] = '0' + VEHICLE_ID / 10;
  address[4] = '0' + VEHICLE_ID % 10;
  address[5] = 0;
}

int8_t readJoystickAxis(uint8_t pin, bool invert) {
  int16_t raw = analogRead(pin);
  if (abs(raw - 512) <= JOYSTICK_DEADZONE) return 0;
  int16_t value = map(raw, 0, 1023, -100, 100);
  if (invert) value = -value;
  return (int8_t)constrain(value, -100, 100);
}

int8_t readTiltAxis(float deltaG, int8_t direction) {
  float magnitude = fabs(deltaG);
  if (magnitude <= TILT_DEADZONE_G) return 0;
  float scaled = (magnitude - TILT_DEADZONE_G) * 100.0f /
                 (TILT_FULL_G - TILT_DEADZONE_G);
  int16_t value = (int16_t)min(scaled, 100.0f);
  if (deltaG < 0.0f) value = -value;
  return (int8_t)(value * direction);
}

void processSerialCommands() {
  while (Serial.available()) {
    char command = Serial.read();
    bool valid = true;
    switch (command) {
      case 'w': throttle = 60; steering = 0; break;
      case 's': throttle = -60; steering = 0; break;
      case 'a': throttle = 0; steering = -60; break;
      case 'd': throttle = 0; steering = 60; break;
      case 'q': throttle = 60; steering = -40; break;
      case 'e': throttle = 60; steering = 40; break;
      case 'z': throttle = -60; steering = -40; break;
      case 'c': throttle = -60; steering = 40; break;
      case 'x': case 'X': case ' ': throttle = 0; steering = 0; break;
      default: valid = false; break;
    }
    if (valid) {
      serialCommandActive = true;
      lastSerialCommandMs = millis();
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MODE_SWITCH, INPUT_PULLUP);
  pinMode(10, OUTPUT); // Uno hardware SS remains an output.
  digitalWrite(10, HIGH);
  Wire.begin();
  imu.setWakeTimeoutMs(100);
  imuReady = imu.beginI2C(Wire, 0x68);
  if (!imuReady) imuReady = imu.beginI2C(Wire, 0x69);
  Serial.print(F("BMI270: "));
  Serial.println(imuReady ? F("ready") : F("not found; tilt mode stops"));

  if (!radio.begin()) {
    Serial.println(F("nRF24L01 not found. Check 3.3 V adapter and SPI."));
    while (true) delay(1000);
  }
  uint8_t address[6];
  makeRadioAddress(address);
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPayloadSize(sizeof(ControlPacket));
  radio.openWritingPipe(address);
  radio.stopListening();
#if USE_JOYSTICK
  Serial.println(F("D4 open=joystick, D4-GND closed=tilt; center each mode first."));
#else
  Serial.println(F("Serial practice: w/s/a/d, q/e/z/c, x=stop; 800 ms hold."));
#endif
}

void loop() {
#if USE_JOYSTICK
  // KCD1 is a mode selector, not a battery or motor-power switch.
  uint8_t mode = digitalRead(PIN_MODE_SWITCH) == LOW ? 1 : 0;
  if (mode != lastMode) {
    lastMode = mode;
    throttle = 0;
    steering = 0;
    joystickCentered = false;
    tiltZeroValid = false;
  }
  while (Serial.available()) Serial.read();
  if (mode == 0) {
    int8_t nextThrottle = readJoystickAxis(A1, true);
    int8_t nextSteering = readJoystickAxis(A0, false);
    if (nextThrottle == 0 && nextSteering == 0) joystickCentered = true;
    throttle = joystickCentered ? nextThrottle : 0;
    steering = joystickCentered ? nextSteering : 0;
  } else if (imuReady && imu.update()) {
    Vec3 accel = imu.accelG();
    lastAccelX = accel.x;
    lastAccelY = accel.y;
    if (!tiltZeroValid) {
      tiltZeroX = accel.x;
      tiltZeroY = accel.y;
      tiltZeroValid = true;
      throttle = 0;
      steering = 0;
    } else {
      throttle = readTiltAxis(accel.y - tiltZeroY, TILT_THROTTLE_SIGN);
      steering = readTiltAxis(accel.x - tiltZeroX, TILT_STEERING_SIGN);
    }
  } else {
    throttle = 0;
    steering = 0;
    tiltZeroValid = false;
  }
#else
  processSerialCommands();
  if (serialCommandActive && (uint32_t)(millis() - lastSerialCommandMs) >= SERIAL_HOLD_MS) {
    throttle = 0;
    steering = 0;
    serialCommandActive = false;
  }
#endif

  if ((uint32_t)(millis() - lastSendMs) >= SEND_INTERVAL_MS) {
    lastSendMs = millis();
    ControlPacket packet = {0xA5, VEHICLE_ID, throttle, steering, sequence++};
    lastAck = radio.write(&packet, sizeof(packet));
  }
  if ((uint32_t)(millis() - lastReportMs) >= 500) {
    lastReportMs = millis();
#if USE_JOYSTICK
    Serial.print(F("mode=")); Serial.print(lastMode == 0 ? F("JOY") : F("TILT"));
    Serial.print(F(" imu=")); Serial.print(imuReady ? F("OK") : F("NO"));
    if (lastMode == 1) {
      Serial.print(F(" ax=")); Serial.print(lastAccelX, 2);
      Serial.print(F(" ay=")); Serial.print(lastAccelY, 2);
    }
#else
    Serial.print(F("mode=SERIAL"));
#endif
    Serial.print(F(" id=")); Serial.print(VEHICLE_ID);
    Serial.print(F(" cmd=")); Serial.print((int)throttle);
    Serial.print(','); Serial.print((int)steering);
    Serial.print(F(" ack=")); Serial.println(lastAck ? F("YES") : F("NO"));
  }
}
