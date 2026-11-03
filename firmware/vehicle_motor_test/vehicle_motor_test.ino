// Arduino Mega 2560 + L298N: short, serial-triggered motor test.
// Connect 12 V ONLY after confirming both motors are rated for 12 V.
// 5V-EN jumper ON; ENA/ENB jumpers OFF; Mega powered by USB.

const uint8_t PIN_ENA = 5;
const uint8_t PIN_IN1 = 22;
const uint8_t PIN_IN2 = 23;
const uint8_t PIN_IN3 = 24;
const uint8_t PIN_IN4 = 25;
const uint8_t PIN_ENB = 6;

const uint8_t TEST_PWM = 120;       // 0..255; begin with the wheels raised
const uint32_t TEST_TIME_MS = 600;
uint32_t testStartedMs = 0;
bool testRunning = false;

void oneMotor(uint8_t en, uint8_t inA, uint8_t inB, int8_t direction) {
  analogWrite(en, 0);
  digitalWrite(inA, direction > 0 ? HIGH : LOW);
  digitalWrite(inB, direction < 0 ? HIGH : LOW);
  analogWrite(en, direction == 0 ? 0 : TEST_PWM);
}

void stopMotors() {
  oneMotor(PIN_ENA, PIN_IN1, PIN_IN2, 0);
  oneMotor(PIN_ENB, PIN_IN3, PIN_IN4, 0);
}

void setup() {
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_ENB, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  stopMotors();

  Serial.begin(115200);
  Serial.println(F("Wheels raised? Send 1=A forward, 2=B forward, 3=both forward, 4=both reverse, x=stop."));
  Serial.println(F("Each command runs for 600 ms, then stops."));
}

void loop() {
  while (Serial.available()) {
    char command = Serial.read();
    if (command == 'x' || command == 'X') {
      stopMotors();
      testRunning = false;
      Serial.println(F("STOP"));
    } else if (command >= '1' && command <= '4') {
      stopMotors();
      if (command == '1' || command == '3') oneMotor(PIN_ENA, PIN_IN1, PIN_IN2, 1);
      if (command == '2' || command == '3') oneMotor(PIN_ENB, PIN_IN3, PIN_IN4, 1);
      if (command == '4') {
        oneMotor(PIN_ENA, PIN_IN1, PIN_IN2, -1);
        oneMotor(PIN_ENB, PIN_IN3, PIN_IN4, -1);
      }
      testStartedMs = millis();
      testRunning = true;
      Serial.print(F("Running test "));
      Serial.println(command);
    }
  }

  if (testRunning && (uint32_t)(millis() - testStartedMs) >= TEST_TIME_MS) {
    stopMotors();
    testRunning = false;
    Serial.println(F("Timed stop"));
  }
}
