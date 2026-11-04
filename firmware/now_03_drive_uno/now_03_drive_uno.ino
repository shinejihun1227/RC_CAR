// Uno USB remote: left joystick VRx=A0 drives forward/back;
// right joystick VRy=A3 turns left/right. BMI and switch are not used.
#include <SPI.h>
#include <RF24.h>
const bool FORWARD_WHEN_A0_HIGH = true; // Check with now_00_joystick_uno.
const bool LEFT_WHEN_A3_HIGH = false; // Check with now_00_joystick_uno.
const int DEADZONE = 60;
const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
struct Packet { byte magic; int8_t throttle; int8_t turn; byte sequence; };
static_assert(sizeof(Packet) == 4, "Packet layout must match Mega");
RF24 radio(7, 8);
bool centeredOnce = false;
byte sequence = 0;
unsigned long lastSendMs = 0, lastPrintMs = 0;
bool lastAck = false;
int8_t lastThrottle = 0, lastTurn = 0;

int8_t axis(int raw, bool positiveWhenHigh) {
  int delta = raw - 512;
  if (abs(delta) <= DEADZONE) return 0;
  int value = map(abs(delta), DEADZONE, 511, 0, 100);
  value = constrain(value, 0, 100);
  return (delta > 0) == positiveWhenHigh ? value : -value;
}
void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); digitalWrite(10, HIGH);
  if (!radio.begin()) {
    Serial.println(F("Radio not found: check 3.3 V and SPI"));
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
  Serial.println(F("Center both sticks once. Positive throttle=forward, turn=left."));
}
void loop() {
  int rawA0 = analogRead(A0);
  int rawA3 = analogRead(A3);
  if (abs(rawA0 - 512) <= DEADZONE && abs(rawA3 - 512) <= DEADZONE) centeredOnce = true;
  lastThrottle = centeredOnce ? axis(rawA0, FORWARD_WHEN_A0_HIGH) : 0;
  lastTurn = centeredOnce ? axis(rawA3, LEFT_WHEN_A3_HIGH) : 0;
  if (millis() - lastSendMs >= 50) {
    lastSendMs = millis();
    Packet p = {0xA5, lastThrottle, lastTurn, sequence++};
    lastAck = radio.write(&p, sizeof(p));
  }
  if (millis() - lastPrintMs >= 500) {
    lastPrintMs = millis();
    Serial.print(F("A0=")); Serial.print(rawA0);
    Serial.print(F(" A3=")); Serial.print(rawA3);
    Serial.print(F(" throttle=")); Serial.print(lastThrottle);
    Serial.print(F(" turn=")); Serial.print(lastTurn);
    Serial.print(F(" ACK=")); Serial.println(lastAck ? F("YES") : F("NO"));
  }
}
