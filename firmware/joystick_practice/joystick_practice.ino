// Arduino Uno: read two 5 V-compatible joystick modules without the radio.
// Joystick 1: VRx=A0, VRy=A1, SW=D2. Joystick 2: VRx=A2, VRy=A3, SW=D3.
// Both modules: VCC=5V, GND=GND. Switches are active LOW with INPUT_PULLUP.
uint32_t lastPrintMs = 0;

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  Serial.println(F("Joystick 1 A0/A1/D2; joystick 2 A2/A3/D3. Center near 512."));
}

void loop() {
  if ((uint32_t)(millis() - lastPrintMs) < 200) return;
  lastPrintMs = millis();
  Serial.print(F("J1 x="));
  Serial.print(analogRead(A0));
  Serial.print(F(" y="));
  Serial.print(analogRead(A1));
  Serial.print(F(" sw="));
  Serial.print(digitalRead(2) == LOW ? F("PRESSED") : F("released"));
  Serial.print(F(" | J2 x="));
  Serial.print(analogRead(A2));
  Serial.print(F(" y="));
  Serial.print(analogRead(A3));
  Serial.print(F(" sw="));
  Serial.println(digitalRead(3) == LOW ? F("PRESSED") : F("released"));
}
