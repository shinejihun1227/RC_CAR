// Arduino Uno: safe I2C identity check for a BMI270 breakout.
// SDA=A4, SCL=A5. Wire power only after verifying breakout voltage/level shifting.
// This does NOT initialize the accelerometer/gyroscope or print motion data.
#include <Wire.h>

bool readChipId(uint8_t address, uint8_t &chipId) {
  Wire.beginTransmission(address);
  Wire.write(0x00); // Bosch BMI270 CHIP_ID register.
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(address, (uint8_t)1) != 1) return false;
  chipId = Wire.read();
  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println(F("BMI270 I2C probe: expected address 0x68/0x69 and CHIP_ID 0x24."));
}

void loop() {
  bool found = false;
  for (uint8_t address = 0x68; address <= 0x69; address++) {
    uint8_t chipId = 0;
    if (!readChipId(address, chipId)) continue;
    found = true;
    Serial.print(F("I2C 0x")); Serial.print(address, HEX);
    Serial.print(F(" CHIP_ID=0x")); Serial.print(chipId, HEX);
    Serial.println(chipId == 0x24 ? F(" BMI270 OK") : F(" unexpected ID"));
  }
  if (!found) Serial.println(F("No BMI270 response; check power, level shifting, SDA/SCL."));
  delay(1000);
}
