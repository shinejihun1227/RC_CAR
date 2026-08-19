# v4 배선

## 전원

```text
AA 배터리 4개 (+) → 스위치 → TB6612FNG #1 VM, TB6612FNG #2 VM
AA 배터리 4개 (-) ─────────→ 두 드라이버 GND와 ESP32 GND
AA 배터리 4개 → 5 V 벅-부스트 → ESP32 5V/VIN
ESP32 3V3 → 두 드라이버 VCC, VL53L1X, 엔코더 로직 전원(3.3 V 호환 시)
```

두 TB6612FNG의 `STBY`는 ESP32 `3V3`에 연결해 항상 활성화합니다. 각 드라이버의 VM-GND 가까이에 470 uF 이상 커패시터를 둡니다.

## 모터 드라이버와 엔코더

| 위치 | TB6612FNG 채널 | IN1 | IN2 | PWM | 엔코더 A |
| --- | --- | ---: | ---: | ---: | ---: |
| 앞왼쪽 (FL) | #1 A | GPIO 18 | GPIO 19 | GPIO 13 | GPIO 34 |
| 앞오른쪽 (FR) | #1 B | GPIO 23 | GPIO 25 | GPIO 14 | GPIO 35 |
| 뒤왼쪽 (RL) | #2 A | GPIO 26 | GPIO 27 | GPIO 16 | GPIO 36 |
| 뒤오른쪽 (RR) | #2 B | GPIO 32 | GPIO 33 | GPIO 17 | GPIO 39 |

각 엔코더의 GND는 ESP32 GND, VCC는 제품 사양에 맞는 3.3 V에 연결합니다. A 채널만 GPIO 34/35/36/39에 연결합니다. 이 네 GPIO는 입력 전용이며 내부 풀업이 없으므로, 엔코더가 오픈 컬렉터 출력이면 A 신호와 3.3 V 사이에 10 kΩ 풀업 저항을 추가합니다.

VL53L1X는 이전과 동일하게 SDA GPIO 21, SCL GPIO 22를 사용합니다.
