// Arduino Uno: read a 5 V-compatible two-axis joystick without the radio.
// VRx=A0, VRy=A1, VCC=5V, GND=GND; SW unused.
uint32_t lastPrintMs = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Joystick A0/A1 ready; center should be near 512."));
}

void loop() {
  if ((uint32_t)(millis() - lastPrintMs) < 200) return;
  lastPrintMs = millis();
  Serial.print(F("VRx="));
  Serial.print(analogRead(A0));
  Serial.print(F(" VRy="));
  Serial.println(analogRead(A1));
}
