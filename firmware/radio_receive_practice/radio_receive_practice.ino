// Arduino Mega 2560: receive radio test packets without connecting motor power.
// nRF24L01+ PA/LNA needs its own regulated 3.3 V supply (or 5 V-input adapter).
#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const uint8_t ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
uint32_t lastReportMs = 0;
uint32_t receivedCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(53, OUTPUT); // Mega hardware SS stays output in SPI master mode.
  digitalWrite(53, HIGH);
  if (!radio.begin()) {
    Serial.println(F("Radio not found: check 3.3 V adapter, CE/CSN, SPI."));
    while (true) delay(1000);
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setPayloadSize(sizeof(uint32_t));
  radio.openReadingPipe(1, ADDRESS);
  radio.startListening();
  Serial.println(F("Radio receiver ready; waiting for Uno."));
}

void loop() {
  while (radio.available()) {
    uint32_t sequence = 0;
    radio.read(&sequence, sizeof(sequence));
    receivedCount++;
    Serial.print(F("Received #"));
    Serial.println(sequence);
  }
  if ((uint32_t)(millis() - lastReportMs) >= 2000) {
    lastReportMs = millis();
    Serial.print(F("Total received: "));
    Serial.println(receivedCount);
  }
}
