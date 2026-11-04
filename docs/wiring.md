# 전체 핀 배선표

**차량 Arduino Mega 2560이 무선 수신기**, **리모컨 Arduino Uno가 송신기**입니다. 두 Arduino 사이에 전원선이나 GND선을 잇지 않습니다. 전원을 끄고 배선한 뒤 [전원 연결](power.md) 순서대로 전압을 확인하세요.

## 차량: 모터·L298N·Mega

LM2596로 L298N의 5 V 제어 전원을 공급하므로 **`5V-EN` 캡을 뺍니다.** `ENA`와 `ENB` 캡도 빼고 Mega에서 PWM 신호를 넣습니다. `5V-EN`은 파란 `12V/GND/5V` 단자 위의 `5VEN` 2핀입니다.

| 연결할 핀/선 | 연결할 곳 | 역할 |
|---|---|---|
| 어댑터 출력 `+` | L298N `12V`, LM2596 `IN+` | 모터 전원과 강압 입력 |
| 어댑터 출력 `−` | L298N `GND`, LM2596 `IN−` | 전원 귀로 |
| LM2596 `OUT+` (5.0 V 측정 후) | L298N `5V` | L298N 제어 회로 전원 |
| LM2596 `OUT−` | L298N `GND` | 강압 출력 GND |
| Mega `GND` | L298N `GND` | 신호 기준 공유 |
| Mega `D5` | L298N `ENA` | A 모터 PWM 속도 |
| Mega `D22` | L298N `IN1` | A 모터 방향 1 |
| Mega `D23` | L298N `IN2` | A 모터 방향 2 |
| Mega `D24` | L298N `IN3` | B 모터 방향 1 |
| Mega `D25` | L298N `IN4` | B 모터 방향 2 |
| Mega `D6` | L298N `ENB` | B 모터 PWM 속도 |
| 왼쪽 모터 두 선 | L298N `OUT1`, `OUT2` | A 모터 출력 |
| 오른쪽 모터 두 선 | L298N `OUT3`, `OUT4` | B 모터 출력 |

Mega는 책상 실습에서 **USB로 켭니다.** 이때 Mega `5V`와 L298N `5V`는 서로 연결하지 않습니다. 모터의 `+`와 `−` 표시는 전원이 고정된 단자라는 뜻이 아닙니다. L298N이 방향에 따라 출력 극성을 바꿉니다. 먼저 왼쪽 `+→OUT1`, `−→OUT2`, 오른쪽 `+→OUT3`, `−→OUT4`로 연결해 바퀴를 띄우고 방향을 확인합니다. 반대로 돌면 해당 모터의 두 선을 바꾸거나 코드의 반전 설정을 바꿉니다. [L298N 단자 설명](https://docs.sunfounder.com/projects/sf-components/en/latest/component_l298n_module.html)

## 차량: VL53L0XV2 거리 센서

| 센서 기판 | Mega | 역할 |
|---|---|---|
| `SDA` | `D20` | I²C 데이터 |
| `SCL` | `D21` | I²C 클록 |
| `GND` | `GND` | 접지 |
| `VIN`/`VCC` | 기판이 5 V 입력형이면 `5V` | 센서 전원 |

센서 기판이 3.3 V 전용이면 5 V에 연결하지 말고 기판 사양에 맞는 전원·I²C 전압 변환을 사용합니다. `XSHUT`과 `GPIO1`은 현재 미사용입니다. 이 센서를 연결하지 않으면 현재 차량 코드는 **전진을 막습니다.** [Arduino Mega I²C 핀](https://docs.arduino.cc/learn/communication/wire/)

## 양쪽 Arduino: nRF24L01+ PA/LNA 무선

각 Arduino에 무선 **본체 한 개**가 필요합니다. 구매 사진의 위쪽 두 장은 **5 V 입력·3.3 V 출력 소켓 어댑터**의 앞·뒷면이고, 아래쪽 두 장은 **무선 본체**의 앞·뒷면입니다. 이 어댑터가 실제로 없다면 본체마다 하나씩 구하거나 동급의 안정적인 3.3 V 전원을 준비하세요. 본체 `VCC`에 **5 V를 직접 넣지 않습니다.** [PA/LNA 모듈 전원 사양](https://www.sirius-pcb.com/download/YTozOntzOjY6Im9iamVjdCI7czo4OiJwcm9kdWN0cyI7czo5OiJmaWxlX3R5cGUiO3M6OToiMjIxNjYiO30%3D)

| 무선 소켓 어댑터/본체 핀 | 차량 Mega | 리모컨 Uno | 역할 |
|---|---|---|---|
| 어댑터 입력 `VCC` (5 V 입력형) | `5V` | `5V` | 어댑터 전원 |
| `GND` | `GND` | `GND` | 접지 |
| `CE` | `D7` | `D7` | 송수신 동작 제어 |
| `CSN`/`CS` | `D8` | `D8` | SPI 칩 선택 |
| `SCK` | `D52` | `D13` | SPI 클록 |
| `MOSI` | `D51` | `D11` | Arduino→무선 데이터 |
| `MISO` | `D50` | `D12` | 무선→Arduino 데이터 |
| `IRQ` | 연결 안 함 | 연결 안 함 | 현재 코드에서 미사용 |

사진의 **HW-237형**과 같은 배치라면 본체의 **부품이 보이는 면을 위로**, **금색 안테나 커넥터를 오른쪽**, **2×4 헤더를 왼쪽**으로 두고 위에서 내려다봅니다. 이 방향에서 제조사 [상면 배치도 2쪽](https://www.sirius-pcb.com/download/YTozOntzOjY6Im9iamVjdCI7czo4OiJwcm9kdWN0cyI7czo5OiJmaWxlX3R5cGUiO3M6OToiMjIxNjYiO30%3D)의 핀 배열은 아래와 같습니다.

```text
부품면 ↑               금색 안테나 커넥터 →
헤더 왼쪽       왼쪽 열    오른쪽 열
맨 위            GND        VCC (3.3 V)
                 CE         CSN
                 SCK        MOSI
맨 아래          MISO       IRQ
```

**납땜면에서 보면 좌우가 반대로 보입니다.** 실제 본체가 이 사진과 같은 HW-237형인지 제조사 그림과 실물을 대조하세요. 가장 쉬운 연결은 핀 이름이 적힌 **5 V 입력형 nRF24L01 소켓 어댑터**를 본체마다 하나씩 쓰는 것입니다. Mega `D53`과 Uno `D10`은 코드에서 SPI 마스터 출력으로 설정하지만 무선 본체에는 연결하지 않습니다. [RF24 Arduino 배선](https://github.com/nRF24/RF24/blob/master/docs/arduino.md)

## 리모컨 Uno: 조이스틱 두 개

| 조이스틱 핀 | 1번 조이스틱(주행) | 2번 조이스틱(로봇팔용 예비) |
|---|---|---|
| `GND` | Uno `GND` | Uno `GND` |
| `5V` | Uno `5V` | Uno `5V` |
| `VRx` | Uno `A0` | Uno `A2` |
| `VRy` | Uno `A1` | Uno `A3` |
| `SW` | Uno `D2` | Uno `D3` |

두 `5V` 선과 두 `GND` 선을 각각 같은 전원에 나눠 연결합니다. 조이스틱 보드의 5 V 입력 사양을 확인하세요. `VRx`는 좌우, `VRy`는 위아래 위치를 0~1023으로 읽습니다. 중앙은 보통 약 512입니다. `SW`는 스틱을 아래로 누르는 버튼으로, `INPUT_PULLUP`에서는 누르면 `LOW`입니다. 현재 전체 주행 코드는 **1번 A0/A1만** 주행에 사용합니다. 2번과 두 버튼은 입력 실습에서만 확인합니다. [아날로그 입력](https://docs.arduino.cc/tutorials/uno-rev3/AnalogReadSerial), [풀업 입력](https://docs.arduino.cc/language-reference/en/variables/constants/inputOutputPullup/)

## 리모컨 Uno: KCD1 모드 스위치

KCD1의 **한 위치에서 서로 도통되는 접점 두 개**를 전원 없이 멀티미터로 찾습니다. 한 접점은 Uno `D4`, 다른 접점은 Uno `GND`입니다. 코드에서 `D4`가 열려 `HIGH`이면 **조이스틱 모드**, 접점이 닫혀 `LOW`이면 **BMI270 기울기 모드**입니다. 표시등이 달린 3핀 제품이면 나머지 핀은 기판 사양을 확인할 때까지 비워 둡니다. **Uno D4에 5 V나 12 V를 직접 넣지 않습니다.** KCD1은 리모컨 전원을 끄는 선에 연결하지 않습니다. [KCD1 형식 예시](https://www.phaseda.com/switch/KCD1.htm)

## 리모컨 Uno: BMI270

BMI270은 가속도와 각속도를 측정합니다. 이번 기울기 모드는 **가속도 X/Y**로 전진·후진과 좌우 회전을 정합니다. Bosch 원칩의 VDD·VDDIO는 최대 3.6 V입니다. **기판 자체가 5 V 입력과 5 V I²C를 지원하는지**에 따라 배선이 달라집니다. [Bosch 데이터시트](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf)

| BMI270 기판 표기 | Uno | 조건 |
|---|---|---|
| `SDA` | `A4/SDA` | 5 V I²C 호환 기판이면 직접 연결 |
| `SCL` | `A5/SCL` | 5 V I²C 호환 기판이면 직접 연결 |
| `GND` | `GND` | 공통 접지 |
| `VIN`/`VCC` | `5V` 또는 `3.3V` | **기판 입력 사양에 따라** 선택; 원칩에 5 V 금지 |
| `CS`/`CSB` | 기판 설명에 따라 I²C 선택 | 기판이 처리하면 추가 연결 불필요 |
| `SDO`/`SA0` | 기판 설명에 따라 주소 선택 | 주소 `0x68` 또는 `0x69` |
| `INT1`/`INT2` | 연결 안 함 | 현재 코드에서 미사용 |

**3.3 V 전용 기판은 Uno A4/A5에 바로 연결하지 않습니다.** 5 V↔3.3 V 양방향 I²C 레벨 변환기를 사용합니다. 기판 모델명 없이 `VIN`의 허용 전압과 자체 신호 변환 여부를 확정할 수 없습니다. Uno의 I²C 핀은 [A4(SDA)와 A5(SCL)](https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf)입니다.
