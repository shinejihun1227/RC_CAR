#include <Wire.h>
#include <SparkFun_BMI270_Arduino_Library.h>
BMI270 bmi;
void setup() {
  Serial.begin(115200); Wire.begin(21, 22);
  while (bmi.beginI2C() != BMI2_OK) { Serial.println("BMI270 not found"); delay(1000); }
}
void loop() {
  bmi.getSensorData();
  Serial.printf("accel_g x=%.3f y=%.3f z=%.3f gyro_dps x=%.2f y=%.2f z=%.2f\n", bmi.data.accelX, bmi.data.accelY, bmi.data.accelZ, bmi.data.gyroX, bmi.data.gyroY, bmi.data.gyroZ);
  delay(50);
}
