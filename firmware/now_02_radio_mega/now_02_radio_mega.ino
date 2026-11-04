// Mega radio receiver. Built-in LED D13 flashes once per received packet.
// No USB cable is needed while running from vehicle power.
#include <SPI.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};

void setup() {
  // Keep both motor channels disabled during the radio-only check.
  pinMode(5, OUTPUT); digitalWrite(5, LOW);  // ENA
  pinMode(6, OUTPUT); digitalWrite(6, LOW);  // ENB
  pinMode(22, OUTPUT); digitalWrite(22, LOW);
  pinMode(23, OUTPUT); digitalWrite(23, LOW);
  pinMode(24, OUTPUT); digitalWrite(24, LOW);
  pinMode(25, OUTPUT); digitalWrite(25, LOW);
  pinMode(13, OUTPUT);
  pinMode(53, OUTPUT); digitalWrite(53, HIGH); // SPI master SS
  if (!radio.begin()) {
    while (true) { digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(100); }
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setPayloadSize(sizeof(uint32_t));
  radio.openReadingPipe(1, ADDRESS);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    uint32_t number;
    while (radio.available()) radio.read(&number, sizeof(number));
    digitalWrite(13, HIGH);
    delay(80);
    digitalWrite(13, LOW);
  }
}
