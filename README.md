# RC카 제작 실습

차량은 Arduino Mega, 빨간 L298N, 37 mm 모터 두 개, VL53L0X 거리 센서로 구성합니다. 조종기에는 Arduino Uno와 nRF24L01 무선 모듈을 사용합니다. [원본 차체 모델](https://makerworld.com/ko/models/1342319-rc-rover-with-robot-arm-6-dof#profileId-1383072)을 바탕으로, 지금 가지고 있는 부품에 맞춰 배선과 코드를 정리했습니다.

먼저 바퀴만 달린 차량 한 대를 완성합니다. 이후 같은 구성을 15대로 늘리고, 로봇팔을 달 차량 한 대는 별도로 확장할 계획입니다. 이 저장소에는 현재 **주행과 전방 거리 감지**까지 담겨 있습니다.

## 실습 순서

1. [배선표](docs/wiring.md)를 보고 Mega, L298N, 모터를 연결합니다. 모터 정격 전압을 확인한 뒤 [모터 동작 코드](firmware/motor_practice/motor_practice.ino)로 양쪽 바퀴의 방향을 맞춥니다.
2. [거리 센서 코드](firmware/distance_practice/distance_practice.ino)로 센서 앞 물체의 거리가 시리얼 모니터에 나오는지 봅니다.
3. 무선 모듈 두 개를 연결하고 [차량 코드](firmware/vehicle_receiver/vehicle_receiver.ino)와 [조종기 코드](firmware/controller_transmitter/controller_transmitter.ino)를 각각 올립니다.
4. 바퀴를 바닥에서 띄운 상태에서 조종 명령, 장애물 앞 정지, 무선 연결이 끊겼을 때 정지를 확인합니다. 자세한 순서는 [실습 진행](docs/practice.md)에 적었습니다.

조이스틱이 없어도 조종기 Uno의 시리얼 모니터에서 `w`(전진), `s`(후진), `a`·`d`(회전), `x`(정지)를 보낼 수 있습니다. 나중에 조이스틱을 연결하면 조종기 코드의 `USE_JOYSTICK`을 `1`로 바꾸면 됩니다.

## 필요한 문서

- [부품과 수량](docs/parts.md): 차량 한 대를 만들 때와 16대로 늘릴 때 필요한 수량
- [배선표](docs/wiring.md): 각 단자와 Arduino 핀의 연결
- [전원 연결](docs/power.md): 지금 쓰는 12 V 어댑터와 향후 3S 배터리 구성
- [제어 방식](docs/control.md): 조종 명령, 좌우 바퀴 속도, 거리 감지, 무선 끊김 처리
- [실습 진행](docs/practice.md): 코드를 올리는 순서와 확인할 결과

## 시작 전 확인

12 V 5 A 어댑터는 **모터가 12 V 정격일 때** 연결합니다. 모터의 스톨 전류도 L298N 채널 허용 범위와 비교해야 합니다. 거리 센서 기판이 5 V 입력을 받는지, 무선 모듈용 3.3 V 소켓 어댑터가 실제로 들어 있는지도 확인해 주세요. 상품 사진만으로는 어댑터 포함 수량을 알 수 없습니다.

책상 위 실습에서는 Mega와 Uno를 USB로 켜고, 12 V 어댑터는 L298N에만 연결합니다. **L298N의 `5V-EN` 캡은 끼우고 `ENA`·`ENB` 캡은 뺍니다.** 이 상태에서 Mega `5V`와 L298N `5V`는 연결하지 않습니다. 3S 배터리를 사용할 때는 [전원 연결](docs/power.md)의 별도 구성을 따릅니다.

## 코드 확인 기록

2026-11-03에 Mega용 스케치 세 개와 Uno용 송신 스케치를 컴파일했습니다. Uno 송신기는 시리얼 입력과 조이스틱 입력 설정을 각각 컴파일했습니다. 사용한 환경은 `arduino-cli 1.2.0`, `arduino:avr 1.8.6`, RF24 `f026bea`, Pololu VL53L0X `9f3773c`입니다. 실제 모터 방향과 센서 동작은 실물 배선 후 확인해야 합니다.

Arduino IDE의 라이브러리 관리자에서 **RF24**와 **VL53L0X (Pololu)**를 설치하면 됩니다. `SPI`와 `Wire`는 Arduino 기본 라이브러리입니다.
