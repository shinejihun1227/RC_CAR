// Mega: two motors with radio timeout stop. Distance sensor is not used in this stage.
#include <SPI.h>
#include <RF24.h>
const byte ENA = 5, IN1 = 22, IN2 = 23, IN3 = 24, IN4 = 25, ENB = 6;
const bool LEFT_INVERT = false;  // Match the result of now_01_motor_mega.
const bool RIGHT_INVERT = false; // Change to true if right wheel runs backward.
const byte MAX_PWM = 150;
const unsigned long RADIO_TIMEOUT_MS = 250;
const byte ADDRESS[6] = {'R', 'C', '0', '0', '1', 0};
struct Packet { byte magic; int8_t throttle; int8_t turn; byte sequence; };
static_assert(sizeof(Packet) == 4, "Packet layout must match Uno");
RF24 radio(7, 8);
unsigned long lastPacketMs = 0;
unsigned long lastDriveMs = 0;
bool linked = false;
int8_t throttle = 0, turn = 0;

void motor(byte en, byte a, byte b, int value, bool invert) {
  value = constrain(value, -100, 100);
  if (invert) value = -value;
  analogWrite(en, 0);
  digitalWrite(a, value > 0 ? HIGH : LOW);
  digitalWrite(b, value < 0 ? HIGH : LOW);
  analogWrite(en, (byte)(abs(value) * MAX_PWM / 100));
}
void drive(int left, int right) {
  motor(ENA, IN1, IN2, left, LEFT_INVERT);
  motor(ENB, IN3, IN4, right, RIGHT_INVERT);
}
void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  drive(0, 0);
  pinMode(13, OUTPUT);
  pinMode(53, OUTPUT); digitalWrite(53, HIGH);
  if (!radio.begin()) {
    while (true) { drive(0, 0); digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(100); }
  }
  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setCRCLength(RF24_CRC_16);
  radio.setAutoAck(true);
  radio.setPayloadSize(sizeof(Packet));
  radio.openReadingPipe(1, ADDRESS);
  radio.startListening();
}
void loop() {
  while (radio.available()) {
    Packet p;
    radio.read(&p, sizeof(p));
    if (p.magic == 0xA5 && p.throttle >= -100 && p.throttle <= 100 && p.turn >= -100 && p.turn <= 100) {
      throttle = p.throttle;
      turn = p.turn;
      lastPacketMs = millis();
      linked = true;
    }
  }
  if (millis() - lastDriveMs < 20) return;
  lastDriveMs = millis();
  bool fresh = linked && millis() - lastPacketMs < RADIO_TIMEOUT_MS;
  digitalWrite(13, fresh ? HIGH : LOW);
  if (!fresh) { drive(0, 0); return; }

  // Positive turn means left: slow/reverse left, speed up right.
  int left = (int)throttle - (int)turn;
  int right = (int)throttle + (int)turn;
  int peak = max(abs(left), abs(right));
  if (peak > 100) { left = left * 100 / peak; right = right * 100 / peak; }
  drive(left, right);
}
