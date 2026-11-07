// v2: Uno sends short serial commands to the vehicle over nRF24L01+.
// Serial monitor (115200, no line ending): w/s/a/d/x. Each command
// expires after 700 ms, even if the Uno remains connected.
#include <SPI.h>
#include <RF24.h>

const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
struct Packet { byte magic; int8_t throttle; int8_t turn; byte sequence; };
static_assert(sizeof(Packet) == 4, "Packet must match the Mega sketch");
RF24 radio(7, 8); // CE, CSN.
int8_t throttle = 0, turn = 0;
byte sequence = 0;
bool lastAck = false;
unsigned long lastCommandMs = 0, lastSendMs = 0, lastReportMs = 0;
const unsigned long COMMAND_HOLD_MS = 700;

void readCommand() {
  while (Serial.available()) {
    char key = Serial.read();
    switch (key) {
      case 'w': throttle = 100; turn = 0; break;
      case 's': throttle = -100; turn = 0; break;
      case 'a': throttle = 0; turn = 100; break;
      case 'd': throttle = 0; turn = -100; break;
      case 'x': throttle = 0; turn = 0; break;
      default: continue;
    }
    lastCommandMs = millis();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); digitalWrite(10, HIGH); // Uno hardware SS.
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
  Serial.println(F("Send w=forward s=reverse a=left d=right x=stop"));
}

void loop() {
  readCommand();
  if (millis() - lastCommandMs >= COMMAND_HOLD_MS) throttle = turn = 0;
  if (millis() - lastSendMs >= 50) {
    lastSendMs = millis();
    Packet p = {0xA5, throttle, turn, sequence++};
    lastAck = radio.write(&p, sizeof(p));
  }
  if (millis() - lastReportMs >= 500) {
    lastReportMs = millis();
    Serial.print(F("cmd=")); Serial.print((int)throttle);
    Serial.print(','); Serial.print((int)turn);
    Serial.print(F(" ACK=")); Serial.println(lastAck ? F("YES") : F("NO"));
  }
}
