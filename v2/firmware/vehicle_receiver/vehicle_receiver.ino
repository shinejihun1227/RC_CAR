// 필요 라이브러리: Pololu VL53L1X
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <VL53L1X.h>

constexpr int STBY = 25, AIN1 = 26, AIN2 = 27, PWMA = 14;
constexpr int BIN1 = 32, BIN2 = 33, PWMB = 13;
constexpr uint16_t STOP_MM = 200;
VL53L1X tof;
struct ControlPacket { int16_t throttle, steering; bool emergencyStop; uint32_t sequence; };
volatile ControlPacket latest{}; volatile unsigned long lastReceiveMs = 0;
void motor(int a, int b, int pwmPin, int value) {
  digitalWrite(a, value > 0 ? HIGH : LOW);
  digitalWrite(b, value < 0 ? HIGH : LOW);
  analogWrite(pwmPin, constrain(abs(value), 0, 255));
}
void drive(int left, int right) { motor(AIN1, AIN2, PWMA, left); motor(BIN1, BIN2, PWMB, right); }
bool obstacleTooClose() { const uint16_t mm = tof.read(); return tof.timeoutOccurred() || mm < STOP_MM; }
void onReceive(const esp_now_recv_info_t*, const uint8_t* data, int len) {
  if (len == sizeof(ControlPacket)) { memcpy((void*)&latest, data, sizeof(latest)); lastReceiveMs = millis(); }
}
void setup() {
  for (int p : {STBY, AIN1, AIN2, PWMA, BIN1, BIN2, PWMB}) pinMode(p, OUTPUT);
  drive(0, 0); digitalWrite(STBY, HIGH);
  Wire.begin(21, 22); tof.setTimeout(500);
  if (!tof.init()) while (true) { drive(0, 0); delay(1000); }
  tof.setDistanceMode(VL53L1X::Long); tof.startContinuous(50);
  WiFi.mode(WIFI_STA); esp_now_init(); esp_now_register_recv_cb(onReceive);
}
void loop() {
  if (millis() - lastReceiveMs > 300 || latest.emergencyStop) { drive(0, 0); return; }
  if (latest.throttle > 0 && obstacleTooClose()) { drive(0, 0); return; }
  const int left = constrain(latest.throttle + latest.steering, -255, 255);
  const int right = constrain(latest.throttle - latest.steering, -255, 255);
  drive(left, right);
}
