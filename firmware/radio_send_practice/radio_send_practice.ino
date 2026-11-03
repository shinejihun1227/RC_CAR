// Arduino Uno: send one radio test packet every 500 ms; no joystick needed.
// nRF24L01+ PA/LNA needs its own regulated 3.3 V supply (or 5 V-input adapter).
#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const uint8_t ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
uint32_t sequence = 0;
uint32_t lastSendMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); // Uno hardware SS stays output in SPI master mode.
  digitalWrite(10, HIGH);
  if (!radio.begin()) {
    Serial.println(F("Radio not found: check 3.3 V adapter, CE/CSN, SPI."));
    while (true) delay(1000);
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPayloadSize(sizeof(uint32_t));
  radio.openWritingPipe(ADDRESS);
  radio.stopListening();
  Serial.println(F("Radio sender ready; sending every 500 ms."));
}

void loop() {
  if ((uint32_t)(millis() - lastSendMs) < 500) return;
  lastSendMs = millis();
  bool acknowledged = radio.write(&sequence, sizeof(sequence));
  Serial.print(F("Sent #"));
  Serial.print(sequence++);
  Serial.print(F(" ack="));
  Serial.println(acknowledged ? F("YES") : F("NO"));
}
