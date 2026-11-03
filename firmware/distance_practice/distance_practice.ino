// Arduino Mega 2560 + a VL53L0X breakout. No motor supply is needed.
// SDA=D20, SCL=D21; verify the breakout's VIN rating before connecting power.

#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
uint32_t lastPrintMs = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sensor.setTimeout(100);

  if (!sensor.init()) {
    Serial.println(F("VL53L0X not found. Check VIN, GND, SDA=D20, SCL=D21."));
    while (true) delay(1000);
  }

  sensor.startContinuous(100);
  Serial.println(F("VL53L0X ready; distance in mm. Move an object in front of it."));
}

void loop() {
  if ((uint32_t)(millis() - lastPrintMs) < 100) return;
  lastPrintMs = millis();

  uint16_t mm = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred() || mm == 0 || mm == 65535) {
    Serial.println(F("Distance read failed"));
  } else {
    Serial.print(F("Distance: "));
    Serial.print(mm);
    Serial.println(F(" mm"));
  }
}
