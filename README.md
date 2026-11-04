# RC카 제작 실습

**현재 조립 상태(스위치·BMI270 없이 조이스틱으로 주행)**는 [지금 연결한 부품으로 주행 실습](docs/now-drive-practice.md)부터 진행하세요. 아래의 기존 전원 설명은 다른 배선 방식입니다.

[원본 차체 모델](https://makerworld.com/ko/models/1342319-rc-rover-with-robot-arm-6-dof#profileId-1383072)을 바탕으로, 현재 가진 빨간 L298N·Arduino Mega 2560·Uno·37 mm 모터 두 개·nRF24L01+ PA/LNA 두 개·조이스틱 두 개·BMI270·KCD1·12 V 5 A 어댑터·LM2596에 맞춰 배선과 코드를 정리했습니다. 차량은 Mega가 무선 **수신**, 리모컨은 Uno가 **송신**합니다. 우선 바퀴 주행과 거리 센서 정지를 확인하고, 로봇팔 기능은 나중에 추가합니다.

## 먼저 읽을 문서

- [전체 핀 배선표](docs/wiring.md): L298N, 두 모터, 무선 8핀, 조이스틱 두 개, KCD1, BMI270, 거리 센서
- [전원 연결](docs/power.md): 12 V 어댑터와 LM2596, 이후 3S 배터리 구성
- [실습 순서](docs/practice.md): Mega·Uno에 단계별로 올릴 코드와 예상 결과
- [제어 방식](docs/control.md): 조이스틱/기울기 모드, 거리 제한, 무선 끊김 정지
- [부품과 수량](docs/parts.md): 차량 15대와 로봇팔 차량 1대 기준 수량

## 실습에서 쓰는 코드

| 실습 | Mega | Uno |
|---|---|---|
| 모터 | [motor_practice](firmware/motor_practice/motor_practice.ino) | 없음 |
| 거리 센서 | [distance_practice](firmware/distance_practice/distance_practice.ino) | 없음 |
| 무선 통신 | [radio_receive_practice](firmware/radio_receive_practice/radio_receive_practice.ino) | [radio_send_practice](firmware/radio_send_practice/radio_send_practice.ino) |
| 조이스틱·KCD1 | 없음 | [joystick_practice](firmware/joystick_practice/joystick_practice.ino) |
| BMI270 칩 확인 | 없음 | [bmi270_probe_practice](firmware/bmi270_probe_practice/bmi270_probe_practice.ino) |
| BMI270 움직임 | 없음 | [bmi270_motion_practice](firmware/bmi270_motion_practice/bmi270_motion_practice.ino) |
| 전체 주행 | [vehicle_receiver](firmware/vehicle_receiver/vehicle_receiver.ino) | [controller_transmitter](firmware/controller_transmitter/controller_transmitter.ino) |

Arduino IDE에 **RF24**, **VL53L0X (Pololu)**, **[NiusIMU 0.4.1](https://github.com/dunknowcoding/ArduinoNRF-IMU/tree/v0.4.1)** 라이브러리를 설치합니다. NiusIMU는 GitHub ZIP을 내려받아 Arduino IDE의 `.ZIP 라이브러리 추가`로 설치할 수 있습니다. 일반 SparkFun BMI270 라이브러리 예제는 Uno의 SRAM 한도를 넘어서 이 코드에는 쓰지 않습니다.

## 전원과 주행에서 꼭 맞출 것

책상 실습에서는 12 V 어댑터를 L298N `12V/GND`와 LM2596 입력에 나눠 연결합니다. LM2596 출력을 **연결 전에 멀티미터로 5.0 V**에 맞춰 L298N `5V/GND`에 넣습니다. 이 구성에서는 L298N **`5V-EN` 캡을 빼고**, `ENA`·`ENB` 캡도 뺍니다. Mega와 Uno는 각각 USB로 켭니다. Mega `GND`와 L298N `GND`만 공유하고 두 보드의 `5V`는 연결하지 않습니다. 모터가 12 V 정격이고 스톨 전류가 L298N에 맞는지 먼저 확인합니다.

무선 본체에는 5 V를 직접 넣지 않습니다. 5 V 입력·3.3 V 출력 소켓 어댑터가 각 무선 본체에 한 개씩 필요합니다. BMI270 기판은 **5 V 입력·I²C 호환 여부를 확인**해야 합니다. 3.3 V 전용 기판이라면 Uno와 사이에 양방향 I²C 레벨 변환기를 넣습니다. KCD1은 Uno `D4/GND`의 **모드 선택 접점**으로 사용합니다. 열린 위치는 조이스틱, 닫힌 위치는 BMI270 기울기 조종입니다.

## 코드 검수 기록

2026-11-04에 Mega·Uno 스케치를 컴파일했습니다. NiusIMU 0.4.1(`8756bc1`)을 사용한 Uno 전체 송신기는 **프로그램 25,672 B(79%)·전역 메모리 833 B(40%)**로 컴파일됐습니다. SparkFun BMI270 기본 예제는 전역 메모리 **8,992 B**로 Uno의 2,048 B 한도를 넘었습니다. 최종 차량 코드는 거리 센서가 없거나 값을 못 읽으면 전진을 막고, 무선 명령이 끊기면 정지합니다. 최종 리모컨 코드는 KCD1 모드를 바꿀 때 명령을 0으로 만들고, BMI270 오류가 나면 기울기 모드에서 0을 보냅니다. 실제 배선·전압·모터 방향·센서 방향·무선 연결은 아직 실물로 검증해야 합니다.
