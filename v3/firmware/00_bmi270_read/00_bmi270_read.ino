// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>

#include <Wire.h>
#include <SparkFun_BMI270_Arduino_Library.h>
// 라이브러리가 제공하는 IMU 객체. bmi.으로 센서 기능을 호출한다.
BMI270 bmi;
void setup(void) {
  Serial.begin(115200);
  Wire.begin(21, 22);
  while (bmi.beginI2C() != BMI2_OK) {
    Serial.println("BMI270 not found");
    delay(1000);
  }
}
void loop(void) {
  bmi.getSensorData();
  Serial.printf("accel_g x=%.3f y=%.3f z=%.3f gyro_dps x=%.2f y=%.2f z=%.2f\n", bmi.data.accelX,
                bmi.data.accelY, bmi.data.accelZ, bmi.data.gyroX, bmi.data.gyroY, bmi.data.gyroZ);
  delay(50);
}
