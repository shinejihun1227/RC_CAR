# 실제 연결과 시험 순서

각 `.ino` 파일은 **파일명과 같은 이름의 폴더**에서 Arduino IDE로 엽니다. 시리얼 모니터는 전부 **115200 bps**입니다. 네 스케치는 Mega/Uno 대상으로 **컴파일을 확인**했지만, **실물 차량으로 검증했다고 주장하지 않습니다.**

## 0. 전원 투입 전 확인

1. 37 mm 모터 라벨/판매 사양에서 **정격 전압과 스톨 전류** 확인. 12 V 모터가 아니면 12 V 어댑터 모터 시험을 중단하고 모터에 맞는 전원 설계부터 수정.
2. VL53L0XV2 **실물 기판이 5 V VIN을 받는지** 확인. nRF24L01용 작은 소켓 어댑터가 **5 V 입력형**인지 확인. 어댑터가 없다면 본체를 Mega/Uno 5 V에 직접 꽂지 않음.
3. L298N `5V-EN` **끼움**, `ENA/ENB` **뺌**. Mega `5V`↔L298N `5V` 선은 없음. Mega `GND`↔L298N `GND`는 있음.
4. 두 바퀴를 바닥에서 띄우고, 헐거운 배선·노출된 도체가 닿지 않게 정리.

## 1. 모터 단품

- Mega에 [vehicle_motor_test.ino](../firmware/vehicle_motor_test/vehicle_motor_test.ino) 업로드.
- Mega USB 전원을 연결한 뒤, 정격이 확인된 경우에만 12 V 모터 전원을 L298N에 연결.
- 시리얼에 `1` → 모터 A 0.6초, `2` → 모터 B 0.6초, `3` → 양쪽 전진 0.6초, `4` → 양쪽 후진 0.6초, `x` → 즉시 정지.
- 전진에서 실제 좌우 바퀴가 같은 차량 진행 방향으로 도는지 확인. 반대면 해당 모터의 두 선을 바꾸거나 통합 코드의 방향 반전 값을 바꿈.
- 코드가 멈춘 뒤에도 바퀴가 계속 돌면 **즉시 12 V 전원을 분리**하고 `ENA/ENB` 점퍼와 배선 확인.

## 2. 거리 센서 단품

- **모터 12 V 전원은 빼고**, Mega USB와 센서만 연결.
- [vehicle_distance_test.ino](../firmware/vehicle_distance_test/vehicle_distance_test.ino) 업로드.
- 센서 앞의 물체를 이동하면서 `Distance: ... mm` 값이 변하는지 확인.
- `not found`면 VIN/GND/SDA D20/SCL D21과 센서 기판 전압을 확인.

## 3. 무선 연결

- **무선 본체 2개**와 각각에 맞는 안정적인 3.3 V 공급을 준비. 안테나를 단단히 연결.
- 무선 연결만 먼저 확인하려면 RF24 라이브러리의 [GettingStarted 예제](https://github.com/nRF24/RF24/blob/master/examples/GettingStarted/GettingStarted.ino)를 각각 Mega와 Uno에 업로드하고 CE=D7, CSN=D8로 설정.
- 또는 바로 Mega에 [vehicle_receiver.ino](../firmware/vehicle_receiver/vehicle_receiver.ino), Uno에 [controller_transmitter.ino](../firmware/controller_transmitter/controller_transmitter.ino) 업로드. **12 V 모터 전원은 아직 빼고**, 두 보드를 USB로 켭니다.
- 두 코드의 `VEHICLE_ID=1`, `RADIO_CHANNEL=76`이 같아야 함. Uno 시리얼에 `w`를 보내면 Uno는 `ack=YES`, Mega는 `link=OK`, `cmd=60,0`을 보여야 함. 센서가 장애물 근처거나 미연결이면 `blocked=1`이므로 모터 12 V를 나중에 넣어도 전진이 제한됨.
- Uno에 `ack=NO`이면 안테나, 어댑터 전원, SPI 핀, CE/CSN, 두 코드의 ID·채널을 확인.

## 4. 통합 주행

- 거리 센서와 무선 연결이 둘 다 정상일 때만 모터 정격에 맞는 전원을 L298N에 연결.
- 바퀴를 띄운 채 Uno 시리얼에 `w` 전진, `s` 후진, `a/d` 제자리 회전, `q/e` 전진 좌우, `z/c` 후진 좌우, `x` 정지를 한 글자씩 전송.
- 각 명령이 **0.8초 안에 자동 정지**하는지 확인. 조종기 USB 또는 무선 전원을 끊었을 때도 차량이 **약 0.3초 이후 정지**하는지 확인.
- 센서 앞 300 mm 안에 장애물을 두고 `w`를 보내면 두 바퀴의 전진이 막히는지 확인. `s` 후진은 가능해야 함. 장애물을 400 mm 밖으로 옮기면 다시 전진 가능해야 함.
- 바닥 주행은 처음에 낮은 출력으로 진행하고, 실제 정지 거리를 재서 통합 코드의 `STOP_DISTANCE_MM`, `RESUME_DISTANCE_MM`, `MAX_PWM`을 조정.

## 조이스틱을 나중에 추가할 때

Uno의 VRx=A0, VRy=A1, VCC=5 V, GND=GND 연결을 확인한 뒤 송신 코드의 `USE_JOYSTICK`을 `1`로 변경합니다. 조이스틱을 놓았을 때 시리얼에 `cmd=0,0`에 가까운 값이 나오는지 **모터 전원 없이** 먼저 확인합니다. 방향이 반대면 송신 코드의 축 반전이나 차량 코드의 모터 반전을 조정합니다.

## 흔한 증상

| 증상 | 먼저 확인할 것 |
|---|---|
| Mega는 켜졌는데 모터만 안 돎 | 12 V 입력, 모터 정격, L298N `5V-EN`, `ENA/ENB` 점퍼 제거, 공통 GND |
| 모터가 계속 최고속으로 돎 | `ENA/ENB` 점퍼가 아직 꽂혔는지 |
| 거리값 없음 | 센서 기판 전압, SDA D20 / SCL D21, 공통 GND |
| 무선 모듈 감지 실패 | 본체 3.3 V 공급, 어댑터 입력 전압, SPI 배선, CE D7 / CSN D8 |
| `ack=YES`인데 움직이지 않음 | 센서 `blocked=1`, 명령 자동 만료, 모터 12 V 전원, 차량 상태 출력 |
| 방향 반대 | 모터 선 한 쌍 또는 코드의 반전 상수 한 곳만 변경 |
