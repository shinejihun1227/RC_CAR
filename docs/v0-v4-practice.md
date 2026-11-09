# RC카 Ready·v0~v4 실습 순서

차량은 Mega 2560, 조종기는 Uno다. 각 버전 폴더의 `.ino`를 Arduino IDE에서 열어 보드와 포트를 선택해 올린다. Mega와 Uno에는 각각 해당 버전의 파일을 올린다. 시리얼 모니터 속도는 모두 **115200 baud**다. 처음에는 바퀴를 공중에 띄운다.

## 먼저 확인할 배선과 전원

| 부품 | 연결 |
|---|---|
| L298N 모터 전원 | 12 V 어댑터 `+→12V`, `−→GND` |
| L298N 제어 | Mega `D5→ENA`, `D22→IN1`, `D23→IN2`, `D24→IN3`, `D25→IN4`, `D6→ENB`, `GND→GND` |
| 모터 | 왼쪽 `OUT1/OUT2`, 오른쪽 `OUT3/OUT4` |
| 거리센서 | Mega `D20→SDA`, `D21→SCL`, `GND→GND`; 전원은 **센서 기판의 허용 전압**에 맞춤 |
| 차량 무선 | Mega `D7→CE`, `D8→CSN`, `D52→SCK`, `D51→MOSI`, `D50→MISO`, `GND→GND` |
| 조종기 무선 | Uno `D7→CE`, `D8→CSN`, `D13→SCK`, `D11→MOSI`, `D12→MISO`, `GND→GND` |
| 왼쪽 조이스틱 | Uno `VRx→A0`, `VRy→A1`, `SW→D2`, `5V→5V`, `GND→GND` |
| 오른쪽 조이스틱 | Uno `VRx→A2`, `VRy→A3`, `SW→D3`, `5V→5V`, `GND→GND` |
| KCD1(2핀 접점) | v4에서 Uno `D4↔GND`; 접점이 닫히면 기울기 모드 |
| BMI270 | v4에서 Uno `A4→SDA`, `A5→SCL`, `GND→GND`; 전원·I²C 전압은 **해당 기판 사양**에 맞춤 |

L298N의 `ENA`·`ENB` 점퍼를 빼고 PWM 선을 연결한다. `5V-EN`은 별개의 점퍼다. **Mega를 USB로 켜는 책상 실습에서는 L298N `5V`와 Mega `5V`를 잇지 않는다.** L298N의 5V-EN 점퍼가 꽂혀 있으면 모듈 자체 제어 회로는 12V에서 5V를 만들 수 있다. Mega GND와 L298N GND는 공통이어야 한다. 업로드할 때 12V를 끄고, 업로드 후 12V를 켠다. v0은 전원을 켠 다음 Mega의 RESET을 눌러 일회성 모터 동작을 다시 시작할 수 있다.

**무선이 필요한 v2부터** 안테나형 nRF24L01+ PA/LNA 본체마다 안정된 **별도 3.3V 전원**을 준비한다. Uno/Mega의 내장 3.3V 핀만으로 PA형 모듈을 공급하지 않는다. 차량의 LM2596을 3.3V로 맞춘다면 입력은 12V에서 분기하고, 리모컨에는 별도 3.3V 전원 장치가 필요하다. 무선 본체 `VCC`에 5V를 넣지 않는다. 5V 입력형 무선 소켓 어댑터를 사용한다면 그 제품의 실제 전원·신호 변환 사양을 확인한다. 맨 모듈에 직접 연결할 때는 Uno/Mega의 `CE`, `CSN`, `SCK`, `MOSI` 5V 신호를 3.3V로 변환한다. `MISO`는 모듈→Arduino 방향이다. IRQ는 연결하지 않는다. [RF24 배선 안내](https://github.com/nRF24/RF24/blob/master/docs/arduino.md), [RF24 전원 점검](https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md)

센서 기판의 `VCC`가 5V 입력을 받는지, I²C 핀이 5V 신호를 견디는지는 기판마다 다르다. **확인되지 않은 3.3V 전용 기판을 Mega/Uno의 5V 핀이나 I²C에 직접 연결하지 않는다.** 37mm 모터의 정격 전압과 정지 전류도 확인한다. L298N의 한쪽 채널이 아직 안 도는 상태라면 v0을 통과하기 전 다음 버전으로 진행하지 않는다.

## 버전별 업로드와 확인

| 버전 | 업로드 파일 | 실행·통과 기준 |
|---|---|---|
| **Ready** 모터+거리 | Mega [`ready.ino`](../firmware/ready/ready.ino) | **v0 앞의 통합 시연**. 바퀴를 띄우고 센서 전방에 30cm보다 먼 물체를 둔다. 왼쪽 전진 2초 → 정지 0.4초 → 왼쪽 후진 2초 → 정지 → 오른쪽 전진 2초 → 정지 → 오른쪽 후진 2초 → 정지를 반복한다. 30cm 이하에서는 두 모터 모두 멈추고, 30cm를 넘으면 멈춘 단계부터 이어진다. 센서 오류 때도 정지한다. Mega USB로 시리얼 모니터를 **115200 baud**로 열면 `Distance: 35.2 cm | Motor: RUN` 형태의 값이 약 0.2초마다 나온다. USB로 Mega에 전원을 공급할 때는 LM2596→Mega `5V` 선을 분리하고, 모터용 12V는 L298N에 별도로 공급한다. |
| **v0** 모터 | Mega [`vehicle_mega.ino`](../firmware/v0/vehicle_mega/vehicle_mega.ino) | 12V 모터 정격을 확인하고 바퀴를 띄운다. RESET 뒤 약 2초 후 왼쪽 전진 → 오른쪽 전진 → 양쪽 전진 → 양쪽 후진을 각 0.5초씩 **한 번** 실행한다. 두 바퀴가 모두 돌아야 통과한다. 반대 방향이면 `LEFT_INVERT`/`RIGHT_INVERT`를 바꾼다. `TEST_PWM=200`은 현재 진단값이며 모터 사양에 맞춰 조정한다. |
| **v1** 거리+모터 | Mega [`vehicle_mega.ino`](../firmware/v1/vehicle_mega/vehicle_mega.ino) | Mega 시리얼 모니터에서 `distance_mm`이 물체 이동에 따라 바뀌는지 본다. `w` 전진, `s` 후진, `a` 좌회전, `d` 우회전, `x` 정지. `w/s/a/d`는 v0의 최대 PWM으로 짧게 시험하고, 명령은 700ms 후 자동 정지한다. 30cm 이내에서 `blocked=YES`가 되고 전진/제자리 회전은 멈추며 후진만 가능해야 한다. 40cm 이상에서 해제된다. 센서 오류도 전진을 막는다. |
| **v2** 무선 시리얼 조종 | Mega [`vehicle_mega.ino`](../firmware/v2/vehicle_mega/vehicle_mega.ino), Uno [`controller_uno.ino`](../firmware/v2/controller_uno/controller_uno.ino) | 두 보드에 업로드하고 무선 전원을 확인한다. Uno 시리얼 모니터에서 `ACK=YES`를 확인한 뒤 `w/s/a/d/x`를 보낸다. `w/s/a/d`는 차량 코드의 최대 PWM으로 짧게 시험한다. 명령은 700ms 후 중립으로 돌아가고, 무선 패킷이 끊기면 차량은 약 250ms 후 정지한다. v1의 거리 제한도 유지된다. |
| **v3** 조이스틱 조종 | Mega [`vehicle_mega.ino`](../firmware/v3/vehicle_mega/vehicle_mega.ino), Uno [`controller_uno.ino`](../firmware/v3/controller_uno/controller_uno.ino) | 두 스틱을 **한 번 가운데에 놓은 뒤**, 왼쪽 `A0`을 전진 방향으로, 오른쪽 `A3`을 좌회전 방향으로 움직인다. Uno 화면에서 `throttle` 양수=전진, `turn` 양수=좌회전, `ACK=YES`여야 한다. 방향이 거꾸로면 아래의 부호 설정을 수정한다. |
| **v4** 스위치·BMI270 | Mega [`vehicle_mega.ino`](../firmware/v4/vehicle_mega/vehicle_mega.ino), Uno [`controller_uno.ino`](../firmware/v4/controller_uno/controller_uno.ino) | Uno 화면에서 `BMI270=OK` 확인. KCD1 접점 열림은 `mode=JOY`, 닫힘은 `mode=TILT`. 스위치를 바꾸면 먼저 정지한다. 기울기 모드로 바꿀 때 조종기를 수평으로 잡아 기준 각도를 잡고, 작은 기울기로 방향을 확인한다. BMI가 없거나 읽기 실패 시 기울기 모드 명령은 0이다. |

시리얼 명령은 Arduino IDE 시리얼 모니터 입력창에 한 글자를 넣고 **전송**한다. 줄바꿈 문자는 무시한다. v2~v4 차량 코드는 동일하다. 같은 거리 제한·무선 패킷 형식을 유지하므로 한 버전에서 다음 버전으로 넘어갈 때 차량 파일도 해당 폴더에서 다시 올리면 절차를 따라가기 쉽다.

## 방향이 반대로 움직이면

- v0의 `LEFT_INVERT`와 `RIGHT_INVERT` 결과를 v1~v4 차량 파일의 같은 설정에 복사한다. `true`는 해당 바퀴의 회전 방향을 뒤집는다.
- v3·v4 조종기에서 왼쪽 스틱을 원하는 전진 방향으로 밀 때 `A0`가 512보다 커지면 `FORWARD_WHEN_A0_HIGH=true`, 작아지면 `false`다. 오른쪽 스틱을 원하는 좌회전 방향으로 밀 때 `A3`가 커지면 `LEFT_WHEN_A3_HIGH=true`, 작아지면 `false`다.
- v4 기울기 모드의 전후·좌우 방향은 BMI270 장착 방향에 따라 달라진다. 바퀴를 띄운 상태에서 `TILT_FORWARD_SIGN` 또는 `TILT_LEFT_SIGN`를 `-1`/`1`로 바꿔 맞춘다. 모드를 바꾸는 순간 기준을 잡으므로 조종기를 수평으로 유지한다.
- 차량 센서는 **전방**을 향해야 한다. 장애물 30cm·재개 40cm는 코드의 `STOP_DISTANCE_MM`·`RESUME_DISTANCE_MM`에서 바꾼다. 실제 바닥 주행 전에는 정지 거리를 실측한다.
- v3·v4에서 스틱을 조금만 움직이면 PWM도 낮아 모터가 떨기만 할 수 있다. 바퀴를 띄우고 큰 입력부터 확인한다. v0의 최대 PWM에서도 한쪽이 돌지 않으면 먼저 배선·ENA·모터 드라이버 채널을 점검한다.

## 설치 라이브러리와 전원 분리

Ready와 v1~v4 Mega에는 Arduino IDE 라이브러리 관리자에서 **VL53L0X (Pololu)**를 설치한다. v2~v4 양쪽 보드에는 **RF24 (TMRh20/nRF24)**가 필요하다. v4 Uno에는 [NiusIMU 0.4.1](https://github.com/dunknowcoding/ArduinoNRF-IMU/tree/v0.4.1)을 ZIP 라이브러리로 설치한다. 일반적인 SparkFun BMI270 라이브러리는 기존 Uno 빌드에서 RAM 한도를 넘어서 이 실습의 v4에는 쓰지 않았다.

v2~v4를 차량 USB 없이 실행하려면 Mega에 안정된 별도 5V 공급이 필요하다. 현재의 `L298N 5V→Mega 5V` 방식은 모터 기동 때 전압이 떨어지지 않는지 실물에서 확인해야 한다. **Mega 5V 핀으로 전원을 넣을 때 USB와 동시에 공급하지 않는다.** 5V를 Mega `VIN`에 넣는 방식은 사용하지 않는다. 차와 조종기 사이에는 전원선이나 GND선을 잇지 않는다.

