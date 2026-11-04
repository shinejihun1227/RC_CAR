// Uno radio transmitter. Open serial monitor at 115200; ACK=YES means Mega replied.
#include <SPI.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
uint32_t number = 0;

void setup() {
  Serial.begin(115200);
  pinMode(10, OUTPUT); digitalWrite(10, HIGH); // SPI master SS
  if (!radio.begin()) {
    Serial.println(F("Radio not found: check power and wiring"));
    while (true) delay(1000);
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPayloadSize(sizeof(uint32_t));
  radio.openWritingPipe(ADDRESS);
  radio.stopListening();
}
void loop() {
  bool ok = radio.write(&number, sizeof(number));
  Serial.print(F("packet=")); Serial.print(number++);
  Serial.print(F(" ACK=")); Serial.println(ok ? F("YES") : F("NO"));
  delay(500);
}
