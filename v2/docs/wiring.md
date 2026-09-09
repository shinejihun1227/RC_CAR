# v2 조종기 배선

| 부품 핀 | 조종기 ESP32 | 설명 |
|---|---|---|
| 조이스틱 VRx | GPIO 34 | 조향 ADC |
| 조이스틱 VRy | GPIO 35 | 전후진 ADC |
| 조이스틱 VCC/GND | 3.3V/GND | ADC 입력이 3.3V를 넘지 않도록 연결 |
| 정지 버튼 | GPIO 4와 GND | INPUT_PULLUP, 눌림 LOW |

차량은 [v0 모터 배선](../../v0/docs/wiring.md)과 [v1 센서 배선](../../v1/docs/wiring.md)을 유지한다. 조종기의 GPIO 34/35와 v4 차량의 엔코더 GPIO 34/35는 서로 다른 ESP32의 핀이다.
