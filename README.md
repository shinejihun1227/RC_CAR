# Mega + Uno 무선 RC카 실험

이 저장소는 **차량 Arduino Mega 2560 + 빨간 L298N + 37 mm DC 모터 2개 + VL53L0X(V2) 거리 센서 + nRF24L01+ PA/LNA**와 **조종기 Arduino Uno + 두 번째 nRF24L01+**를 위한 배선, 제어 원리, 실험 코드입니다. [원본 차체 모델](https://makerworld.com/ko/models/1342319-rc-rover-with-robot-arm-6-dof#profileId-1383072)을 참고하되, 여기의 배선과 코드는 현재 가지고 있다고 알려준 **빨간 L298N**에 맞춰 새로 정했습니다.

지금 구현하는 것은 **바퀴 주행과 전방 거리 제한**입니다. 로봇팔 서보 7개의 제어·전원은 아직 포함하지 않았습니다. 15대의 바퀴 전용 차량과 1대의 로봇팔 차량을 만들 계획이라면, 먼저 이 회로로 **차량 1대**를 실물 검증한 뒤 복제하세요.

> **전원을 넣기 전에:** 37 mm 모터의 **정격 전압과 시동/정지(스톨) 전류**를 확인하세요. 정격이 12 V가 아닌 모터에 12 V 어댑터를 연결하면 안 됩니다. L298N의 모터 채널은 무한정 큰 전류를 처리하지 못합니다. 실제 모터 사양이 아직 확인되지 않았으므로 이 저장소는 실물 주행 검증 완료 상태가 아닙니다.

## 저장소 안내

| 문서·코드 | 내용 |
|---|---|
| [부품 목록](docs/parts.md) | 보유 부품, 추가로 필요한 부품, 16대 확장 수량 |
| [전체 핀맵](docs/wiring.md) | Mega·Uno·L298N·거리 센서·무선 모듈·선택 조이스틱 배선 |
| [전원 구성](docs/power.md) | 12 V 어댑터 시험과 향후 3S 배터리 전원 구분 |
| [제어 원리](docs/control.md) | 패킷, 차동 주행, 거리 제한, 무선 끊김 정지, 다수 차량 설정 |
| [실험 순서](docs/test-plan.md) | 단품 확인부터 통합 주행까지 관찰할 결과 |
| [Mega 모터 단품 코드](firmware/vehicle_motor_test/vehicle_motor_test.ino) | 시리얼 명령으로 0.6초씩 모터 시험 |
| [Mega 거리 센서 코드](firmware/vehicle_distance_test/vehicle_distance_test.ino) | 거리(mm) 시리얼 출력 |
| [Mega 통합 수신기](firmware/vehicle_receiver/vehicle_receiver.ino) | 무선 수신, 거리 제한, L298N 제어 |
| [Uno 통합 송신기](firmware/controller_transmitter/controller_transmitter.ino) | 기본: 시리얼 키 명령. 선택: 조이스틱 입력 |

## 준비

1. Arduino IDE에서 보드를 각각 **Arduino Mega 2560**과 **Arduino Uno**로 선택합니다.
2. 라이브러리 관리자에서 **RF24**(nRF24 프로젝트)와 **VL53L0X**(Pololu)를 설치합니다. `SPI`, `Wire`는 Arduino 기본 라이브러리입니다. [RF24 원본](https://github.com/nRF24/RF24), [Pololu VL53L0X 원본](https://github.com/pololu/vl53l0x-arduino)
3. [전체 핀맵](docs/wiring.md)과 [전원 구성](docs/power.md)을 보고 배선합니다. 사진 속 상품의 `1PC` 선택은 모듈 한 개일 수 있으므로, **무선 링크에는 본체 2개**와 각 본체에 맞는 안정적인 **3.3 V 공급**이 필요합니다.
4. [실험 순서](docs/test-plan.md)에 따라 모터, 거리 센서, 무선, 통합 주행 순으로 확인합니다.

## 가장 먼저 실행할 시험

- 모터 정격이 12 V라고 확인되면, 바퀴를 바닥에서 띄우고 Mega에 `vehicle_motor_test.ino`를 업로드합니다. 시리얼 모니터 **115200 bps**에서 `1`, `2`, `3`, `4`를 하나씩 전송합니다. 명령마다 약 **0.6초** 움직인 뒤 멈춥니다.
- 거리 센서는 모터 전원 없이 `vehicle_distance_test.ino`로 먼저 시험합니다.
- 무선 모듈 두 개가 준비되면 Mega와 Uno에 통합 스케치를 각각 올립니다. 조이스틱이 없어도 Uno 시리얼 모니터에서 `w` 전진, `s` 후진, `a`/`d` 회전, `x` 정지를 전송할 수 있습니다. 명령은 **0.8초 뒤 자동으로 0**이 됩니다. 차량은 유효한 무선 명령이 **0.3초 동안 없으면 정지**합니다.

## 현재 확정되지 않은 것

- 37 mm 모터의 실제 **정격 전압과 스톨 전류**
- VL53L0XV2 **실물 기판의 VIN 허용 전압**
- 구매한 무선 상품에 **소켓형 3.3 V 어댑터가 포함됐는지와 개수**
- 조이스틱 보유 여부, 실제 바퀴 좌우 방향, 차량 16대의 개별 배터리 규격

이 항목은 제품 라벨·실측으로 확인한 뒤 [전원 구성](docs/power.md)과 코드 상단 설정을 조정해야 합니다. 코드의 핀 배치와 정지 거리 300/400 mm는 **첫 시험용 설계값**입니다.

## 확인 범위

2026-11-03에 `arduino-cli 1.2.0`, `arduino:avr 1.8.6`, RF24 소스 `f026bea`, Pololu VL53L0X 소스 `9f3773c`로 **네 스케치를 모두 컴파일**했습니다. Mega용 모터·거리·수신기 3개와 Uno용 송신기의 **시리얼·조이스틱 두 입력 모드**가 문법·라이브러리 연결 단계에서 통과했습니다. **실물 배선, 무선 통신, 모터 방향, 장애물 정지는 아직 시험하지 않았습니다.**

## 참고 자료

- [Arduino Mega 2560 핀 도면](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf), [Uno Rev3 핀 도면](https://docs.arduino.cc/resources/pinouts/A000066-full-pinout.pdf)
- [SunFounder 빨간 L298N 모듈 설명](https://docs.sunfounder.com/projects/sf-components/en/latest/component_l298n_module.html)
- [Nordic nRF24L01 사양서](https://devzone.nordicsemi.com/cfs-file/__key/support-attachments/beef5d1b77644c448dabff31668f3a47-aad1a46f307945a7b0204fd969e86bdf/content.pdf)
- [ST VL53L0X 사양서](https://www.st.com/resource/en/datasheet/vl53l0x.pdf)

기존 ESP32 실습 내용은 이 커밋에서 현재 트리에서 제거했습니다. 이전 내용이 필요하면 Git 커밋 이력에서 확인할 수 있습니다.
