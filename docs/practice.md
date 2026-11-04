# 실습 순서와 업로드할 코드

차량은 **Arduino Mega 2560**, 리모컨은 **Arduino Uno**로 Arduino IDE에서 선택합니다. 시리얼 모니터는 모두 **115200 bps**입니다. `.ino` 파일은 같은 이름의 폴더째 엽니다. 코드 업로드 때는 Mega·Uno를 각각 PC USB로 켭니다. [전체 핀 배선표](wiring.md)와 [12 V·LM2596 전원 연결](power.md)을 먼저 봅니다.

라이브러리 관리자에서 **RF24**와 **VL53L0X (Pololu)**를 설치합니다. BMI270 기울기 실습과 최종 Uno 송신기에는 **[NiusIMU 0.4.1](https://github.com/dunknowcoding/ArduinoNRF-IMU/tree/v0.4.1)**이 필요합니다. GitHub의 `Code → Download ZIP`으로 내려받아 Arduino IDE `스케치 → 라이브러리 포함하기 → .ZIP 라이브러리 추가`에서 설치할 수 있습니다. `SPI`와 `Wire`는 Arduino 기본 라이브러리입니다. 일반 SparkFun BMI270 라이브러리의 기본 예제는 Uno의 SRAM 2 KB를 초과했으므로 이 저장소의 Uno 코드에는 사용하지 않습니다.

| 순서 | Mega에 올릴 코드 | Uno에 올릴 코드 | 확인할 것 |
|---|---|---|---|
| 0. 전원 | 없음 | 없음 | LM2596 출력 5.0 V, 점퍼 상태 |
| 1. 모터 | [motor_practice.ino](../firmware/motor_practice/motor_practice.ino) | 없음 | 두 바퀴 방향·0.6초 자동 정지 |
| 2. 거리 | [distance_practice.ino](../firmware/distance_practice/distance_practice.ino) | 없음 | 거리 mm 값 변화 |
| 3. 무선 | [radio_receive_practice.ino](../firmware/radio_receive_practice/radio_receive_practice.ino) | [radio_send_practice.ino](../firmware/radio_send_practice/radio_send_practice.ino) | 송신 `ack=YES`, 수신 번호 증가 |
| 4. 입력 | 없음 | [joystick_practice.ino](../firmware/joystick_practice/joystick_practice.ino) | 조이스틱 두 개·버튼·KCD1 모드 |
| 5. BMI270 확인 | 없음 | [bmi270_probe_practice.ino](../firmware/bmi270_probe_practice/bmi270_probe_practice.ino) | 주소 `0x68/0x69`, 칩 ID `0x24` |
| 6. BMI270 움직임 | 없음 | [bmi270_motion_practice.ino](../firmware/bmi270_motion_practice/bmi270_motion_practice.ino) | 가속도 g·각속도 deg/s 변화 |
| 7. 전체 주행 | [vehicle_receiver.ino](../firmware/vehicle_receiver/vehicle_receiver.ino) | [controller_transmitter.ino](../firmware/controller_transmitter/controller_transmitter.ino) | 조이스틱/기울기 주행, 정지 조건 |

새 스케치를 업로드하면 그 보드의 이전 코드가 바뀝니다. **3번 코드는 서로 짝**, **7번 코드도 서로 짝**입니다. 3번 송신기와 7번 수신기를 섞지 않습니다.

## 0. 전원부터 확인

모터 라벨에서 **12 V 정격**인지 확인하고, 두 모터의 스톨 전류가 L298N 허용치 안인지 확인합니다. 어댑터·LM2596를 연결한 뒤 **LM2596 출력에 다른 부품을 꽂기 전에** `OUT+`/`OUT−`가 5.0 V인지 멀티미터로 측정합니다. 전원을 다시 끄고 L298N `5V-EN` 캡을 뺀 다음 LM2596 출력 → L298N `5V/GND`를 연결합니다. `ENA`·`ENB` 캡도 뺍니다. Mega와 Uno는 각각 USB로 켭니다. USB 5 V와 L298N 5 V는 서로 잇지 않고 GND만 공유합니다.

## 1. 모터 방향

바퀴를 바닥에서 띄우고 Mega에 `motor_practice.ino`를 올립니다. 시리얼 모니터에 `1`을 보내면 A 모터, `2`는 B 모터, `3`은 양쪽 전진, `4`는 양쪽 후진을 **0.6초**만 구동합니다. `x`는 즉시 정지입니다. 전진 방향이 반대인 모터는 해당 모터의 두 선을 맞바꾸거나 최종 코드의 모터 반전 설정을 바꿉니다. 모터 전압·스톨 전류를 아직 모르면 이 단계에서 12 V를 인가하지 않습니다.

## 2. 거리 센서

모터 전원을 끄고 Mega의 거리 센서만 연결합니다. 센서 기판의 입력 전압을 확인한 뒤 `distance_practice.ino`를 올립니다. 물체를 움직였을 때 `Distance: ... mm`가 변해야 합니다. `VL53L0X not found`면 전원·GND·SDA D20·SCL D21을 확인합니다. 현재 최종 차량 코드는 이 센서가 없거나 읽기 실패 시 **전진을 막고 후진만 허용**합니다.

## 3. 무선 두 개

각 Arduino에 nRF24L01+ 본체와 **각각의 3.3 V 전원 소켓 어댑터**를 연결합니다. 모터 전원은 꺼 둡니다. Mega에 `radio_receive_practice.ino`, Uno에 `radio_send_practice.ino`를 올립니다. Uno에서 `Sent #... ack=YES`, Mega에서 `Received #...` 번호가 0.5초마다 증가하면 통신됩니다. `ack=NO`면 두 본체의 3.3 V 전원과 CE·CSN·SPI 선을 먼저 확인합니다.

## 4. 조이스틱 두 개와 KCD1

1번 조이스틱은 Uno `A0/A1/D2`, 2번은 `A2/A3/D3`에 연결합니다. 각 보드의 `5V/GND`도 연결합니다. KCD1의 스위치 접점 두 개는 Uno `D4/GND`에 연결합니다. `joystick_practice.ino`를 올리면 두 스틱의 `x/y`, 누름 버튼, `mode=JOYSTICK` 또는 `mode=TILT`가 보입니다. 스틱을 놓으면 두 축이 대략 512이고 움직이면 0~1023으로 변합니다. KCD1의 두 위치에서 모드 글자가 바뀌는지 봅니다. 이 실습은 무선·BMI270 없이도 됩니다.

## 5. BMI270 주소와 칩 확인

**BMI270 기판의 전압·I²C 호환성을 확인한 뒤에만** Uno `A4/A5`를 연결합니다. 3.3 V 전용 기판에는 5 V↔3.3 V I²C 레벨 변환기가 필요합니다. `bmi270_probe_practice.ino`를 올려 `I2C 0x68 CHIP_ID=0x24 BMI270 OK` 또는 `0x69`가 나오는지 확인합니다. 이 코드는 연결과 칩 종류만 확인하며 아직 움직임 값은 읽지 않습니다. [Bosch BMI270 데이터시트](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf)

## 6. BMI270 기울기 값

`bmi270_motion_practice.ino`를 Uno에 올려 가속도 `accel[g]`와 각속도 `gyro[deg/s]`를 봅니다. 보드를 가만히 두면 가속도 세 축 중 중력 방향의 값이 약 ±1 g 근처이고 각속도는 0 근처여야 합니다. 좌우·앞뒤로 기울일 때 X/Y가 어떻게 변하는지 기록합니다. 값이 반대로 움직이면 최종 송신 코드의 `TILT_THROTTLE_SIGN` 또는 `TILT_STEERING_SIGN`을 바꿉니다. BMI270은 기판 방향에 따라 축 방향이 달라서 바퀴를 띄운 채 확인해야 합니다.

## 7. 조이스틱 ↔ 기울기 모드로 전체 주행

Mega에 `vehicle_receiver.ino`, Uno에 `controller_transmitter.ino`를 올립니다. 최종 Uno 코드는 **NiusIMU 라이브러리**가 필요하며 기본값 `USE_JOYSTICK=1`입니다. KCD1 접점이 **열리면 조이스틱 모드**, **닫히면 기울기 모드**입니다. 두 모드 모두 전환 직후에는 0 명령으로 시작합니다. 조이스틱 모드에서는 1번 스틱을 중앙에 한 번 놓아야 움직이고, 기울기 모드에서는 스위치를 바꾼 순간의 BMI270 자세가 기준점이 됩니다. **모드를 바꿀 때 리모컨을 평평하고 움직이지 않게 놓으세요.** 2번 조이스틱과 두 스틱의 `SW`는 아직 로봇팔용 예비 입력입니다.

먼저 **모터 12 V 전원을 끄고** Uno 시리얼의 `mode=JOY`/`TILT`, `cmd=전진값,회전값`, `ack=YES`와 Mega의 `link=OK`를 확인합니다. KCD1이 기울기 모드인데 BMI270 초기화·읽기가 실패하면 Uno는 `cmd=0,0`을 보내야 합니다. 그 다음 바퀴를 띄워 모터 전원을 켭니다. 한 방향씩 천천히 움직여 바퀴 방향을 확인합니다. 조이스틱 축이나 BMI270 축이 반대면 코드의 반전 설정을 조정합니다.

현재 코드는 마지막 유효 무선 명령 뒤 **300 ms**가 지나면 차량을 정지시키고, 거리 센서가 **30 cm 이하**를 읽으면 전진 바퀴를 멈춥니다. **40 cm 이상**에서 전진이 다시 허용됩니다. 센서를 빼거나 읽기가 실패하면 전진이 막힙니다. `USE_JOYSTICK=0`으로 바꿔 Uno 코드를 다시 올리면 KCD1·조이스틱 대신 시리얼 모니터에서 `w/s/a/d`, `x`로 입력하는 실습 모드가 됩니다. 각 시리얼 명령은 **800 ms** 뒤 풀립니다.

차량을 여러 대 만들면 각 Mega의 `VEHICLE_ID`를 1~16으로 다르게 설정하고, Uno의 ID를 조종할 차량과 맞춥니다. 같은 ID로 두 차량을 켜면 함께 명령을 받을 수 있습니다.

## 실습 결과에서 구별할 점

| 증상 | 먼저 확인할 곳 |
|---|---|
| Mega가 켜졌지만 모터가 안 돎 | L298N `5V-EN` 제거, LM2596 `OUT+` 5.0 V, 모터 12 V, `ENA/ENB` 선, 공통 GND |
| `ack=NO` | 무선 본체 2개, 각 3.3 V 전원, CE/CSN/SPI |
| `ack=YES`, `cmd`가 0 | KCD1 모드, 1번 조이스틱 중앙 확인, BMI270 읽기 결과 |
| `ack=YES`, `cmd`는 있는데 전진 안 됨 | Mega의 `blocked` 값, 거리 센서, 모터 전원 |
| BMI270 칩 ID는 확인되는데 움직임 안 나옴 | NiusIMU 설치, BMI270 초기화 결과, 기판 I²C 사양 |

컴파일은 확인했지만 실제 전압·모터 회전 방향·무선 수신과 BMI270 기울기 방향은 실물에서 확인해야 합니다.
