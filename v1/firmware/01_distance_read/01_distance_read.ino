// Arduino IDE 실습: C 문법 중심의 제어 로직. 라이브러리 호출은 Arduino C++ API.
#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>

// 필요 라이브러리: Pololu VL53L1X
#include <Wire.h>
#include <VL53L1X.h>

// 라이브러리가 제공하는 거리센서 객체. tof.으로 센서 기능을 호출한다.
VL53L1X tof;

void setup(void) {
  Serial.begin(115200);
  Wire.begin(21, 22);
  tof.setTimeout(500);
  if (!tof.init()) {
    Serial.println("VL53L1X init failed");
    while (true)
      delay(1000);
  }
  tof.setDistanceMode(VL53L1X::Long);
  tof.setMeasurementTimingBudget(50000);
  tof.startContinuous(50);
}

void loop(void) {
  const uint16_t distanceMm = tof.read();
  if (tof.timeoutOccurred())
    Serial.println("timeout");
  else
    Serial.printf("distance_mm=%u\n", distanceMm);
  delay(50);
}
