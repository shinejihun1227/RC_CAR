// 필요 라이브러리: Pololu VL53L1X
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X tof;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  tof.setTimeout(500);
  if (!tof.init()) {
    Serial.println("VL53L1X init failed");
    while (true) delay(1000);
  }
  tof.setDistanceMode(VL53L1X::Long);
  tof.setMeasurementTimingBudget(50000);
  tof.startContinuous(50);
}

void loop() {
  const uint16_t distanceMm = tof.read();
  if (tof.timeoutOccurred()) Serial.println("timeout");
  else Serial.printf("distance_mm=%u\n", distanceMm);
  delay(50);
}
