// Uno USB serial monitor, 115200 baud. Move left VRx (A0) and right VRy (A3).
// No radio or vehicle is needed for this check.
void setup() { Serial.begin(115200); }

void loop() {
  Serial.print(F("left_X_A0="));
  Serial.print(analogRead(A0));
  Serial.print(F("  right_Y_A3="));
  Serial.println(analogRead(A3));
  delay(200);
}
