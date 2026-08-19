constexpr int JOY_X = 34, JOY_Y = 35, STOP_BUTTON = 4;
void setup() { Serial.begin(115200); pinMode(STOP_BUTTON, INPUT_PULLUP); }
void loop() {
  Serial.printf("x=%d y=%d stop=%d\n", analogRead(JOY_X), analogRead(JOY_Y), !digitalRead(STOP_BUTTON));
  delay(100);
}
