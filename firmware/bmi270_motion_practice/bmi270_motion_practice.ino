// Uno + BMI270: acceleration and rotation test using NiusIMU 0.4.1.
// SDA=A4, SCL=A5. Confirm breakout power and I2C level shifting first.
#include <Wire.h>
#include <BMI270.h>

BMI270 imu;
bool ready = false;
uint32_t lastPrintMs = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  ready = imu.beginI2C(Wire, 0x68);
  if (!ready) ready = imu.beginI2C(Wire, 0x69);
  if (!ready) {
    Serial.print(F("BMI270 init failed: "));
    Serial.println(imu.lastStageText());
    return;
  }
  Serial.println(F("BMI270 ready. Move the board; accel is g, gyro is deg/s."));
}

void loop() {
  if (!ready) {
    delay(1000);
    return;
  }
  if ((uint32_t)(millis() - lastPrintMs) < 100) return;
  lastPrintMs = millis();
  if (!imu.update()) {
    Serial.println(F("BMI270 read failed"));
    return;
  }
  Vec3 a = imu.accelG();
  Vec3 g = imu.gyroDps();
  Serial.print(F("accel[g] x=")); Serial.print(a.x, 2);
  Serial.print(F(" y=")); Serial.print(a.y, 2);
  Serial.print(F(" z=")); Serial.print(a.z, 2);
  Serial.print(F(" gyro[deg/s] x=")); Serial.print(g.x, 1);
  Serial.print(F(" y=")); Serial.print(g.y, 1);
  Serial.print(F(" z=")); Serial.println(g.z, 1);
}
