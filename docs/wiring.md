# 배선표

아래 핀 번호는 저장소의 코드와 같습니다. 전원을 끈 상태에서 연결하고, L298N과 무선 어댑터의 핀 **이름**을 기판에서 확인하세요. 차량 Mega와 조종기 Uno는 무선으로만 통신합니다.

## 차량: L298N과 두 모터

이번 12 V 어댑터 구성에서는 L298N의 `5V-EN` 캡을 **끼우고**, `ENA`와 `ENB` 캡은 **뺍니다**. `5V-EN`은 파란색 `12V/GND/5V` 단자 위쪽의 `5VEN` 2핀입니다.

| 연결할 선 | 도착 단자 |
|---|---|
| 12 V 어댑터 `+` | L298N `12V` |
| 12 V 어댑터 `−` | L298N `GND` |
| Mega `GND` | L298N `GND` |
| Mega `D5` | L298N `ENA` |
| Mega `D22`, `D23` | L298N `IN1`, `IN2` |
| Mega `D24`, `D25` | L298N `IN3`, `IN4` |
| Mega `D6` | L298N `ENB` |
| 왼쪽 모터 두 선 | L298N `OUT1`, `OUT2` |
| 오른쪽 모터 두 선 | L298N `OUT3`, `OUT4` |

모터 선은 먼저 `+`를 `OUT1` 또는 `OUT3`, `−`를 `OUT2` 또는 `OUT4`에 연결합니다. 전진 명령에서 바퀴가 반대로 돌면 해당 모터 선 두 개를 맞바꾸거나 코드의 모터 반전값을 바꿉니다. `OUT3`와 `OUT4`의 위아래 위치는 보드에 적힌 글자를 따르세요.

Mega는 USB로 켭니다. **Mega `5V`와 L298N `5V`는 연결하지 않습니다.** `5V-EN` 캡이 끼워진 L298N의 `5V` 단자는 자체 레귤레이터의 출력입니다. [L298N 점퍼와 단자 설명](https://docs.sunfounder.com/projects/sf-components/en/latest/component_l298n_module.html)

## 차량: 거리 센서

| VL53L0XV2 기판 | Mega |
|---|---|
| `SDA` | `D20` |
| `SCL` | `D21` |
| `GND` | `GND` |
| `VIN` 또는 `VCC` | 기판이 5 V 입력형이면 `5V` |

센서 기판이 3.3 V 전용이면 Mega의 5 V에 연결하지 마세요. `XSHUT`과 `GPIO1` 핀이 있다면 이번 구성에서는 사용하지 않습니다. Mega의 I²C 핀은 D20(SDA), D21(SCL)입니다. [Arduino I²C 핀 안내](https://docs.arduino.cc/learn/communication/wire/)

## 차량과 조종기: 무선 모듈

각 Arduino에 nRF24L01+ 본체 한 개와 3.3 V 전원 어댑터 한 개가 필요합니다. 아래 `VCC`는 **5 V 입력이라고 표시된 소켓형 어댑터의 입력 핀**을 뜻합니다. 무선 모듈 본체 VCC에 5 V를 직접 연결하지 마세요. PA/LNA형 본체의 전원을 Mega·Uno의 3.3 V 핀에서 직접 공급하는 구성도 피합니다. [Nordic 전원 사양](https://devzone.nordicsemi.com/cfs-file/__key/support-attachments/beef5d1b77644c448dabff31668f3a47-aad1a46f307945a7b0204fd969e86bdf/content.pdf), [Mega 3.3 V 출력 한도](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)

| 무선 어댑터/모듈 | 차량 Mega | 조종기 Uno |
|---|---|---|
| 어댑터 `VCC` | `5V` | `5V` |
| `GND` | `GND` | `GND` |
| `CE` | `D7` | `D7` |
| `CSN` 또는 `CS` | `D8` | `D8` |
| `SCK` | `D52` | `D13` |
| `MOSI` | `D51` | `D11` |
| `MISO` | `D50` | `D12` |
| `IRQ` | 연결하지 않음 | 연결하지 않음 |

Mega와 Uno의 SPI 핀 번호가 다릅니다. [RF24의 Arduino 연결 안내](https://github.com/nRF24/RF24/blob/master/docs/arduino.md)

## 조이스틱을 추가할 때

| 조이스틱 | Uno |
|---|---|
| `VRx` | `A0` |
| `VRy` | `A1` |
| `VCC` | `5V` (5 V 호환 모듈일 때) |
| `GND` | `GND` |
| `SW` | 연결하지 않음 |

연결 후 `controller_transmitter.ino`의 `USE_JOYSTICK`을 `1`로 바꿉니다. 조이스틱이 없다면 기본값 `0`을 유지하고 시리얼 모니터로 조종합니다.
