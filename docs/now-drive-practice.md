# 지금 연결한 부품으로 주행 실습

스위치와 BMI270은 연결하지 않은 상태다. 리모컨 Uno는 컴퓨터 USB로 켠다. 차량 Mega는 L298N `5V`에서 전원을 받는다. L298N에는 12V 어댑터를 연결하고 `5V-EN` 캡은 끼운다. 속도 조절을 위해 `ENA`·`ENB` 캡은 뺀다.

| L298N | Mega |
|---|---|
| ENA, IN1, IN2 | D5, D22, D23 |
| IN3, IN4, ENB | D24, D25, D6 |
| GND | GND |
| 5V | 5V (USB와 동시 공급 금지) |
| OUT1/OUT2 | 왼쪽 모터 |
| OUT3/OUT4 | 오른쪽 모터 |

## 무선 실습 전의 전원 조건

안테나형 nRF24L01+PA/LNA는 송신 순간 약 115~130mA가 필요할 수 있다. Mega와 Uno의 내장 3.3V 핀은 각각 최대 50mA다. **01단계에서는 무선 모듈 전원선을 빼고, 02·03단계는 아래처럼 전원을 고친 뒤 진행한다.** 차량은 LM2596의 입력을 **12V 어댑터에서 분기**하고 출력을 3.3V로 맞춰 무선 모듈에 연결한다. 리모컨에도 별도의 3.3V 전원 모듈이 필요하다. 모듈 연결 전 출력 전압을 측정한다. 각 무선 모듈 `VCC-GND` 가까이에 10µF 이상 콘덴서를 두면 전원 흔들림을 줄일 수 있다. [Arduino 전류 한도](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf), [RF24 전원 안내](https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md)

거리센서와 무선 모듈을 **같은 Mega 3.3V 핀에 함께 연결한 상태도 해제**한다. 거리센서 기판이 5V 전원과 5V I²C를 지원하는 것이 확인되면 `VCC→Mega 5V`, `SDA→D20`, `SCL→D21`, `GND→GND`로 옮길 수 있다. 사양이 확인되지 않으면 **01단계부터 센서 전원선을 빼고** 이번 실습을 진행한다. 아래 01~03단계 코드는 거리센서를 사용하지 않는다.

무선 모듈 본체 `VCC`에는 5V를 넣지 않는다. 5V 신호 허용도 확인되지 않았으므로 아두이노에서 무선 모듈로 가는 `CE`, `CSN`, `SCK`, `MOSI`에는 전압 변환을 쓴다. 저항 분배를 사용한다면 **아두이노 출력→1kΩ→모듈 입력**, **모듈 입력→2kΩ→GND**를 각 선마다 구성한다. `MISO`는 직접 연결한다. [PA/LNA 모듈 사양](https://www.ebyte.com/Uploadfiles/Files/2020-12-16/202012161527137067.pdf)

| 무선 모듈 | 차량 Mega | 리모컨 Uno |
|---|---|---|
| VCC | 외부 3.3V | 외부 3.3V |
| GND | Mega GND와 공통 | Uno GND와 공통 |
| CE / CSN | D7 / D8, 전압 변환 | D7 / D8, 전압 변환 |
| SCK / MOSI / MISO | D52 / D51 / D50 | D13 / D11 / D12 |
| IRQ | 연결 안 함 | 연결 안 함 |

SCK·MOSI에도 전압 변환을 넣는다. 위 표의 Mega와 Uno 사이에는 전원선이나 GND선을 잇지 않는다.

## 업로드 방법

각 `.ino` 파일을 Arduino IDE에서 열고 Uno 파일은 **Arduino Uno**, Mega 파일은 **Arduino Mega or Mega 2560**을 선택한다. 02·03단계에는 라이브러리 관리자에서 **RF24 (TMRh20/nRF24)**를 설치한다.

Mega에 업로드할 때는 **12V 어댑터를 끄고 L298N 5V→Mega 5V 선을 뺀 뒤** USB를 연결한다. 업로드가 끝나면 USB를 빼고, 전원이 꺼진 상태에서 5V 선을 다시 연결한 뒤 12V를 켠다. Uno는 컴퓨터 USB에 계속 연결해 시리얼 모니터를 볼 수 있다.

### 00. 조이스틱 방향 확인

[Uno 조이스틱 확인](../firmware/now_00_joystick_uno/now_00_joystick_uno.ino)을 올리고 시리얼 모니터를 **115200 baud**로 연다. 왼쪽 조이스틱을 원하는 전진 방향으로 밀 때 `A0`가 512보다 커지는지, 오른쪽 조이스틱을 원하는 좌회전 방향으로 밀 때 `A3`가 커지는지 기록한다.

### 01. 모터만 확인

[Mega 모터 확인](../firmware/now_01_motor_mega/now_01_motor_mega.ino)을 올린다. **바퀴를 공중에 띄우고** 12V를 켜면 2초 뒤 왼쪽 전진, 오른쪽 전진, 두 바퀴 전진, 두 바퀴 후진을 각각 0.5초씩 **한 번만** 실행하고 멈춘다. 반대로 도는 쪽은 코드 맨 위 `LEFT_INVERT` 또는 `RIGHT_INVERT`를 `true`로 바꿔 재업로드한다. 03 Mega 파일에도 같은 값을 넣는다. 모터가 12V 정격인지와 정지 전류가 드라이버 한도를 넘지 않는지도 확인한다.

### 02. 무선 연결만 확인

전원을 수정한 뒤 [Mega 무선 수신](../firmware/now_02_radio_mega/now_02_radio_mega.ino)을 올리고 차량 전원을 켠다. [Uno 무선 송신](../firmware/now_02_radio_uno/now_02_radio_uno.ino)을 올려 시리얼 모니터를 **115200 baud**로 연다. `ACK=YES`가 계속 나오면 Mega가 수신해 응답한 것이다. Mega 내장 LED(D13)도 패킷을 받을 때 잠깐 켜진다. `ACK=NO`면 양쪽 3.3V, 공통 GND, CE/CSN/SPI 핀부터 확인한다. 이 코드로는 모터가 돌지 않는다.

### 03. 조이스틱으로 스키드 조향

[Mega 차량 수신](../firmware/now_03_drive_mega/now_03_drive_mega.ino)과 [Uno 조종기 송신](../firmware/now_03_drive_uno/now_03_drive_uno.ino)을 각각 올린다. **처음에는 바퀴를 공중에 띄운다.** 두 조이스틱을 가운데에 둔 적이 있어야 주행 명령이 나온다.

- 왼쪽 조이스틱 `VRx→A0`: 사용자 기준 오른쪽으로 밀면 전진, 왼쪽으로 밀면 후진.
- 오른쪽 조이스틱 `VRy→A3`: 보드 위쪽으로 밀면 좌회전, 아래쪽으로 밀면 우회전.
- 좌회전 값이 양수이면 왼쪽 바퀴가 느려지고 오른쪽 바퀴가 빨라진다. 정지 상태에서 조향하면 제자리 회전한다.

00단계에서 전진으로 밀 때 A0가 증가하면 Uno 파일의 `FORWARD_WHEN_A0_HIGH=true`, 감소하면 `false`다. 좌회전으로 밀 때 A3가 증가하면 `LEFT_WHEN_A3_HIGH=true`, 감소하면 `false`다. 01단계에서 확인한 `LEFT_INVERT`와 `RIGHT_INVERT`를 Mega 파일에 그대로 적용한다. Uno 시리얼 모니터에서 `throttle` 양수는 전진, `turn` 양수는 좌회전이고 `ACK=YES`가 나와야 한다.

무선 패킷이 끊기면 Mega는 약 **250ms** 뒤 모터를 멈춘다. 이 단계에는 거리센서의 장애물 정지 기능을 넣지 않았다. 먼저 수동 주행을 확인한 뒤 거리센서 코드를 합친다.
