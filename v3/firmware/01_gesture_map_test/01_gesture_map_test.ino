// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <math.h>

#include <Wire.h>
#include <SparkFun_BMI270_Arduino_Library.h>
// 라이브러리가 제공하는 IMU 객체. bmi.으로 센서 기능을 호출한다.
BMI270 bmi;
const float DEAD_ZONE_DEG = 8.0f;
void setup(void) {
  Serial.begin(115200);
  Wire.begin(21, 22);
  while (bmi.beginI2C() != BMI2_OK)
    delay(1000);
}
void loop(void) {
  bmi.getSensorData();
  const float roll = atan2(bmi.data.accelY, bmi.data.accelZ) * 180.0f / PI;
  const float pitch = atan2(-bmi.data.accelX, sqrt(bmi.data.accelY * bmi.data.accelY +
                                                   bmi.data.accelZ * bmi.data.accelZ)) *
                      180.0f / PI;
  const char *command;
  if (fabsf(pitch) < DEAD_ZONE_DEG && fabsf(roll) < DEAD_ZONE_DEG) {
    command = "STOP";
  } else if (pitch > DEAD_ZONE_DEG) {
    command = "FORWARD";
  } else if (pitch < -DEAD_ZONE_DEG) {
    command = "BACKWARD";
  } else if (roll > 0) {
    command = "RIGHT";
  } else {
    command = "LEFT";
  }
  Serial.printf("roll=%.1f pitch=%.1f command=%s\n", roll, pitch, command);
  delay(50);
}
