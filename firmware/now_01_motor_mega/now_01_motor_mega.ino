// Mega motor check. Upload over USB with 12 V OFF. Then unplug USB, raise wheels,
// and turn on 12 V. Runs once per power-up, then stops.
const byte ENA = 5, IN1 = 22, IN2 = 23, IN3 = 24, IN4 = 25, ENB = 6;
const bool LEFT_INVERT = false;
const bool RIGHT_INVERT = false;
const byte TEST_PWM = 110; // Start slowly.

void motor(byte en, byte a, byte b, int direction, bool invert) {
  if (invert) direction = -direction;
  analogWrite(en, 0);
  digitalWrite(a, direction > 0 ? HIGH : LOW);
  digitalWrite(b, direction < 0 ? HIGH : LOW);
  analogWrite(en, direction == 0 ? 0 : TEST_PWM);
}
void drive(int left, int right) {
  motor(ENA, IN1, IN2, left, LEFT_INVERT);
  motor(ENB, IN3, IN4, right, RIGHT_INVERT);
}
void step(int left, int right) {
  drive(left, right);
  delay(500);
  drive(0, 0);
  delay(700);
}
void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  drive(0, 0);
  delay(2000);
  step(1, 0);  // Left wheel forward
  step(0, 1);  // Right wheel forward
  step(1, 1);  // Both forward
  step(-1, -1); // Both reverse
}
void loop() { drive(0, 0); }
