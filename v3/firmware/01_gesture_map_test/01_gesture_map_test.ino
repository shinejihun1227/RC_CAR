#include <Wire.h>
#include <SparkFun_BMI270_Arduino_Library.h>
BMI270 bmi;
constexpr float DEAD_ZONE_DEG = 8.0f;
void setup() { Serial.begin(115200); Wire.begin(21, 22); while (bmi.beginI2C() != BMI2_OK) delay(1000); }
void loop() {
  bmi.getSensorData();
  const float roll = atan2(bmi.data.accelY, bmi.data.accelZ) * 180.0f / PI;
  const float pitch = atan2(-bmi.data.accelX, sqrt(bmi.data.accelY * bmi.data.accelY + bmi.data.accelZ * bmi.data.accelZ)) * 180.0f / PI;
  const char* command = abs(pitch) < DEAD_ZONE_DEG && abs(roll) < DEAD_ZONE_DEG ? "STOP" : pitch > DEAD_ZONE_DEG ? "FORWARD" : pitch < -DEAD_ZONE_DEG ? "BACKWARD" : roll > 0 ? "RIGHT" : "LEFT";
  Serial.printf("roll=%.1f pitch=%.1f command=%s\n", roll, pitch, command); delay(50);
}
