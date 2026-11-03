# 전체 핀맵: 12 V 어댑터를 이용한 책상 위 시험

**배선 전 모든 전원을 끄고** 단자대의 실크 인쇄를 확인하세요. Mega·Uno는 **각각 USB로 전원**을 받습니다. 차량 Mega와 조종기 Uno 사이에는 전선이 없습니다.

## 1. 차량 전원과 L298N

| 출발 | 도착 |
|---|---|
| 12 V 5 A 어댑터 `+` | L298N `12V` |
| 12 V 5 A 어댑터 `−` | L298N `GND` |
| Mega `GND` | L298N `GND` (어댑터 `−`와 공통) |
| Mega `5V` | **L298N `5V`에 연결하지 않음** |

이 표는 L298N **`5V-EN` 캡을 끼운** 상태입니다. 이때 L298N이 12 V 입력에서 자체 제어용 5 V를 만듭니다. `5V` 나사 단자는 출력이므로 Mega 5 V를 연결하지 않습니다. `5V-EN` 캡은 파란색 `12V/GND/5V` 나사 단자 위, 기판에 `5VEN`이라고 쓰인 작은 2핀입니다. `ENA`와 `ENB` 점퍼캡은 **빼고** Mega의 PWM 핀으로 연결합니다. [모듈의 점퍼 설명](https://docs.sunfounder.com/projects/sf-components/en/latest/component_l298n_module.html)

## 2. 차량 Mega ↔ L298N ↔ 모터

| L298N 단자 | 연결 | 역할 |
|---|---|---|
| `ENA` | Mega **D5** | 왼쪽 모터 PWM 속도 |
| `IN1` | Mega **D22** | 왼쪽 방향 1 |
| `IN2` | Mega **D23** | 왼쪽 방향 2 |
| `IN3` | Mega **D24** | 오른쪽 방향 1 |
| `IN4` | Mega **D25** | 오른쪽 방향 2 |
| `ENB` | Mega **D6** | 오른쪽 모터 PWM 속도 |
| `OUT1/OUT2` | 왼쪽 모터 두 선 | 처음에는 모터 `+`→`OUT1`, `−`→`OUT2` |
| `OUT3/OUT4` | 오른쪽 모터 두 선 | 처음에는 모터 `+`→`OUT3`, `−`→`OUT4` |

`OUT3`와 `OUT4`의 **실제 위치는 기판 인쇄**로 확인하세요. 모터의 `+/-` 표시는 한 방향에 대한 표시일 뿐, L298N이 회전 방향을 바꾸면 출력 극성도 바뀝니다. 바퀴를 띄워 시험하고 전진 명령에서 반대로 도는 쪽이 있으면 모터 선 두 개를 맞바꾸거나 통합 코드의 `LEFT_MOTOR_INVERTED` / `RIGHT_MOTOR_INVERTED`를 바꾸세요. **둘을 동시에 바꾸지는 마세요.**

## 3. 거리 센서 ↔ 차량 Mega

| VL53L0XV2 기판 핀 | Mega |
|---|---|
| `SDA` | **D20 (SDA)** |
| `SCL` | **D21 (SCL)** |
| `GND` | `GND` |
| `VIN` 또는 `VCC` | **실물 기판의 5 V 입력 지원을 확인한 경우에만** Mega `5V` |
| `XSHUT`, `GPIO1`이 있다면 | 첫 시험에서는 연결하지 않음 |

VL53L0X 칩과 판매되는 브레이크아웃 기판의 허용 전압은 같지 않을 수 있습니다. 기판이 `3.3V` 전용이면 Mega 5 V를 바로 연결하지 말고 그 기판에 맞는 전원·레벨 변환을 준비하세요. Mega의 I²C 핀은 SDA D20, SCL D21입니다. [Arduino Wire 핀 표](https://docs.arduino.cc/learn/communication/wire/)

## 4. nRF24L01+ ↔ 차량 Mega

사진 속 PA/LNA 모듈 **본체 VCC는 3.3 V 전용**입니다. 소켓형 전원 어댑터가 있고 **그 어댑터 입력이 5 V라고 명시된 경우**에만 아래 VCC 배선을 쓰세요. 모듈 본체에 5 V나 12 V를 직접 연결하지 않습니다. Mega의 3.3 V 핀에 PA/LNA 모듈을 직접 매다는 것도 권장하지 않습니다. [Nordic 전원 사양](https://devzone.nordicsemi.com/cfs-file/__key/support-attachments/beef5d1b77644c448dabff31668f3a47-aad1a46f307945a7b0204fd969e86bdf/content.pdf), [Mega 3.3 V 핀 한도](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)

| 모듈/5 V 입력형 어댑터 핀 | Mega |
|---|---|
| 어댑터 `VCC` | Mega `5V` |
| `GND` | Mega `GND` |
| `CE` | **D7** |
| `CSN` 또는 `CS` | **D8** |
| `SCK` | **D52** |
| `MOSI` | **D51** |
| `MISO` | **D50** |
| `IRQ` | 연결하지 않음 |

## 5. nRF24L01+ ↔ 조종기 Uno

조종기에도 무선 본체와 안정적인 3.3 V 공급이 **별도로 하나씩** 필요합니다.

| 모듈/5 V 입력형 어댑터 핀 | Uno |
|---|---|
| 어댑터 `VCC` | Uno `5V` |
| `GND` | Uno `GND` |
| `CE` | **D7** |
| `CSN` 또는 `CS` | **D8** |
| `SCK` | **D13** |
| `MOSI` | **D11** |
| `MISO` | **D12** |
| `IRQ` | 연결하지 않음 |

Mega와 Uno의 SPI 핀 번호는 다릅니다. RF24는 하드웨어 SPI 세 선과 CE·CSN 두 선을 사용합니다. [RF24 Arduino 배선](https://github.com/nRF24/RF24/blob/master/docs/arduino.md)

## 6. 선택: 조종기 조이스틱 ↔ Uno

조이스틱이 아직 없다면 이 부분은 건너뛰고 **시리얼 모니터 명령**으로 먼저 시험합니다.

| 2축 조이스틱 핀 | Uno |
|---|---|
| `VRx` | `A0` (좌·우) |
| `VRy` | `A1` (전진·후진) |
| `VCC` | `5V` (5 V 호환 모듈인지 확인) |
| `GND` | `GND` |
| `SW` | 연결하지 않음 |

실제로 연결한 뒤 `controller_transmitter.ino` 상단의 `USE_JOYSTICK`을 `1`로 바꾸세요. 조이스틱 방향이 반대면 `readAxis()`의 반전 설정을 조정합니다.
