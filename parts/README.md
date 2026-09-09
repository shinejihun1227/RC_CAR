# 부품과 신호

| 부품 | 역할 | ESP32와 주고받는 것 | 단계 |
|---|---|---|---|
| ESP32 DevKit V1 | 입력을 읽고 명령·안전 조건을 계산 | GPIO, ADC, I²C, ESP-NOW, USB 시리얼 | 전체 |
| TB6612FNG | 별도 전원으로 모터를 구동 | 방향 디지털 신호와 PWM | 전체 |
| TT 기어드모터 | 2WD 기초 차량 구동 | 드라이버가 공급하는 모터 전력 | v0~v3 |
| VL53L1X | 전방 장애물까지 거리 측정 | I²C, mm 단위 거리값 | v1 이후 차량 |
| 조이스틱 | 전후·좌우 입력 | VRx/VRy 아날로그 전압→ADC 값 | v2 |
| 정지 버튼 | 조종기에서 정지 요청 | GPIO 4, INPUT_PULLUP, 눌림 LOW | v2·v3 |
| BMI270 | 3축 가속도·3축 각속도 측정 | I²C | v3 |
| 엔코더 N20 | 4륜 구동과 회전 펄스 출력 | 모터 전력 입력, CHA 펄스 출력 | v4 |
| AA 4개·5V 벅-부스트 | 모터 전원과 제어보드 전원 공급 | VM과 5V/VIN을 구분 | 차량 |
| 브래킷·샤시·바퀴 | 부품 고정과 움직임 전달 | 장착홀·축·간섭 치수 | 제작 |

모터 전력, 제어 신호, 센서 통신, 무선 명령을 구분해서 배선도를 읽는다. 엔코더 CHA/CHB는 I²C가 아닌 디지털 펄스다.

- [전체 부품표](BOM.md)
- [작동 원리](PRINCIPLES.md)
- 단계별 구매 규격: [v0](purchasing/v0.md), [v1](purchasing/v1.md), [v2](purchasing/v2.md), [v3](purchasing/v3.md), [v4](purchasing/v4.md)
- 실제 핀은 각 버전의 docs/wiring.md를 따른다.
