// Arduino Uno + nRF24L01+ (via a verified 5 V-input / 3.3 V-output adapter).
// Default input is USB Serial Monitor. Joystick is optional: set USE_JOYSTICK=1
// only after VRx=A0, VRy=A1, VCC=5V, GND=GND are actually connected.

#include <SPI.h>
#include <RF24.h>

#define VEHICLE_ID 1       // Must match vehicle_receiver.
#define RADIO_CHANNEL 76  // Must match vehicle_receiver.
#ifndef USE_JOYSTICK
#define USE_JOYSTICK 0    // 0=serial practice, 1=analog joystick.
#endif

const uint8_t PIN_RADIO_CE = 7;
const uint8_t PIN_RADIO_CSN = 8;
const uint32_t SEND_INTERVAL_MS = 50;
const uint32_t SERIAL_HOLD_MS = 800; // Each typed command expires automatically.
const int16_t JOYSTICK_DEADZONE = 60;

struct ControlPacket {
  uint8_t magic;
  uint8_t vehicleId;
  int8_t throttle;
  int8_t steering;
  uint8_t sequence;
};
static_assert(sizeof(ControlPacket) == 5, "Controller and vehicle packet must be 5 bytes");

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);
int8_t throttle = 0;
int8_t steering = 0;
uint8_t sequence = 0;
uint32_t lastSerialCommandMs = 0;
uint32_t lastSendMs = 0;
uint32_t lastReportMs = 0;
bool serialCommandActive = false;
bool lastAck = false;

void makeRadioAddress(uint8_t address[6]) {
  address[0] = 'R';
  address[1] = 'C';
  address[2] = '0';
  address[3] = '0' + VEHICLE_ID / 10;
  address[4] = '0' + VEHICLE_ID % 10;
  address[5] = 0;
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
      default: valid = false; break; // Ignore newline/line-ending characters.
    }
    if (valid) {
      serialCommandActive = true;
      lastSerialCommandMs = millis();
    }
  }
}

int8_t readAxis(uint8_t pin, bool invert) {
  int16_t raw = analogRead(pin);
  if (abs(raw - 512) <= JOYSTICK_DEADZONE) return 0;
  int16_t value = map(raw, 0, 1023, -100, 100);
  if (invert) value = -value;
  return (int8_t)constrain(value, -100, 100);
}

void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); // Uno hardware SS must remain an output in SPI master mode.
  digitalWrite(10, HIGH);
  if (!radio.begin()) {
    Serial.println(F("nRF24L01 not found. Check adapter power and SPI wires."));
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
  Serial.println(F("Controller ready: joystick A0/A1 active; center it before motor power."));
#else
  Serial.println(F("Controller ready: w/s/a/d, q/e/z/c, x=stop. Each command lasts 800 ms."));
#endif
}

void loop() {
#if USE_JOYSTICK
  // Typical joystick: pushing forward lowers VRy. Reverse this sign if needed.
  throttle = readAxis(A1, true);
  steering = readAxis(A0, false);
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
    Serial.print(F("id=")); Serial.print(VEHICLE_ID);
    Serial.print(F(" cmd=")); Serial.print((int)throttle);
    Serial.print(','); Serial.print((int)steering);
    Serial.print(F(" ack=")); Serial.println(lastAck ? F("YES") : F("NO"));
  }
}
