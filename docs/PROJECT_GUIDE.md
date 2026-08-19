# RC_CAR 프로젝트 통합 가이드

이 문서는 현재 결정된 RC_CAR의 학습 흐름, 하드웨어, 전원, 3D 프린팅, 핀맵, 엔코더와 코드 실행 순서를 한 곳에 모은 기준 문서입니다.

> 최종 확장 목표는 **AA 배터리 4개 기반의 4륜 엔코더 N20 RC카**입니다. v0~v3은 기능을 단계적으로 배우기 위한 독립 실습이며, v4에서 차량 구동계를 엔코더 N20 4개 방식으로 확장합니다.

## 1. 전체 학습 흐름

| 버전 | 핵심 학습 | 차량 구조 |
| --- | --- | --- |
| v0 | ESP32, TB6612FNG, PWM 모터 제어 | TT 모터 2개 기반의 기초 실습 |
| v1 | VL53L1X 거리 감지와 안전 정지 | v0 + 전방 ToF 센서 |
| v2 | ESP-NOW, 조이스틱 무선 조종 | 차량 ESP32 + 조종기 ESP32 |
| v3 | BMI270, Roll/Pitch 제스처 | v2 조종기에 BMI270 추가 |
| v4 | 엔코더, RPM 측정, PID 속도 제어 | 엔코더 N20 4개 + TB6612FNG 2개 |

```text
v0 모터 구동
  → v1 거리 안전
    → v2 조이스틱 무선 제어
      → v3 손 제스처 제어
        → v4 엔코더 피드백·PID 속도 보정
```

## 2. 저장소 폴더 구조

```text
RC_CAR/
├─ docs/
│  ├─ PROJECT_GUIDE.md       ← 현재 문서
│  ├─ BOM_TOTAL.md           ← 중복을 제거한 전체 부품표
│  └─ ARDUINO_SETUP.md       ← Arduino IDE 설정
├─ library/                  ← 설치해야 하는 Arduino 라이브러리 안내
├─ market/                   ← 버전별 구매 목록과 검색어
├─ hardware/                 ← 3D CAD·STL·참고 모델의 공통 보관 위치
├─ v0/                       ← 기본 모터 구동 실습
├─ v1/                       ← 거리 안전 정지 실습
├─ v2/                       ← 조이스틱 ESP-NOW 실습
├─ v3/                       ← BMI270 제스처 실습
└─ v4/                       ← 엔코더 N20·PID 실습
```

각 `v0`~`v4` 폴더는 `README.md`, `docs/`, `firmware/`, `hardware/`를 가집니다. 버전별 실습 코드와 배선 문서는 해당 버전 안에서 보고, 공통 사양은 이 문서와 `docs/`를 기준으로 봅니다.

## 3. 최종 차량 하드웨어 구조 (v4)

```text
                조종기 ESP32
       (v2 조이스틱 또는 v3 BMI270)
                       │ ESP-NOW
                       ▼
AA 알카라인 4개 ─→ 차량 ESP32 ─→ TB6612FNG #1 ─→ 앞왼쪽 / 앞오른쪽 N20
       │                 │       TB6612FNG #2 ─→ 뒤왼쪽 / 뒤오른쪽 N20
       │                 │
       ├─→ 5V 벅-부스트 ─┘
       │
       └─→ 모터 VM 전원

VL53L1X ─→ 차량 ESP32 (전방 거리 안전 정지)
엔코더 CHA × 4 ─→ 차량 ESP32 (실제 바퀴 속도 피드백)
```

### 차량 필수 부품

| 분류 | 최종 권장 부품 | 수량 | 역할 |
| --- | --- | ---: | --- |
| MCU | ESP32 DevKit V1 | 1 | 차량 제어·ESP-NOW 수신·PID |
| 구동 | JGA12-N20 홀 엔코더 기어드모터 | 4 | 6V, 약 100RPM, 동일 옵션 4개 |
| 구동 | 3mm D축 호환 바퀴 | 4 | N20 출력축에 결합 |
| 구동 | TB6612FNG | 2 | 모터 4개를 각각 독립 제어 |
| 거리 | VL53L1X ToF 센서 | 1 | 전방 충돌 위험 감지 |
| 전원 | AA 알카라인 배터리 | 4 | 차량 전원, 충전하지 않음 |
| 전원 | 4칸 AA 홀더 + 스위치 | 1 | 약 6V 차량 전원 |
| 전원 | 5V 2A 이상 벅-부스트 컨버터 | 1 | ESP32 5V/VIN 안정화 |
| 보호 | 470uF 이상 16V 전해 커패시터 | 2 | 드라이버별 VM-GND 노이즈 완화 |
| 조립 | N20 브래킷, M3 나사·인서트 | 각 4/1세트 | 모터와 3D 프린트 샤시 체결 |

### 전원 원칙

```text
AA 4개 (+) → 스위치 → TB6612FNG #1/#2 VM
AA 4개 (+/-) → 5V 벅-부스트 입력
5V 벅-부스트 출력 → ESP32 5V 또는 VIN
ESP32 3V3 → TB6612FNG VCC, VL53L1X, 3.3V 호환 엔코더 VCC
모든 GND → 공통 연결
```

- 알카라인 AA 배터리는 충전하지 않습니다.
- 모터 전원과 ESP32 전원의 GND는 반드시 공통입니다.
- 모터가 출발할 때 생기는 전압 흔들림을 줄이기 위해 커패시터를 드라이버 가까이에 둡니다.

## 4. 4륜 스키드 조향

v4는 앞·뒤 바퀴가 서로 다른 조향장치를 갖는 자동차형 구조가 아니라, 네 바퀴를 모두 구동하는 스키드 조향 구조입니다.

```text
앞왼쪽(FL)  ─┐
뒤왼쪽(RL)  ─┼─ 왼쪽 속도 그룹

앞오른쪽(FR) ─┐
뒤오른쪽(RR) ─┼─ 오른쪽 속도 그룹
```

| 동작 | 왼쪽 모터 | 오른쪽 모터 |
| --- | --- | --- |
| 전진 | 정방향 | 정방향 |
| 후진 | 역방향 | 역방향 |
| 좌회전 | 느리게 또는 역방향 | 정방향 |
| 우회전 | 정방향 | 느리게 또는 역방향 |
| 정지 | 정지 | 정지 |

모터의 실제 성능 차이 때문에 모터 4개를 한 드라이버 채널에 병렬로 묶지 않습니다. TB6612FNG 두 개를 사용해 모터 4개를 각각 독립 PWM 제어합니다.

## 5. 엔코더 N20 핵심 개념

### 모터와 감속기

```text
DC 모터 → 금속 감속기 → 3mm D축 출력축 → 바퀴
```

감속기는 모터의 빠른 회전을 낮추고 토크를 높입니다. `6V / 100RPM`은 AA 4개 기반의 작은 3D 프린팅 RC카에서 속도와 힘의 균형을 잡기 좋은 시작점입니다.

### D축과 D컷

```text
원형 축의 한 면을 평평하게 가공한 축 = D축
평평하게 깎인 부분 = D컷
```

선택한 모터 사양은 보통 `3mm D축`, 길이 약 `10mm`입니다. 바퀴는 반드시 `3mm D shaft` 호환 제품을 고르고, 3D 프린팅 허브는 처음에 약 3.1~3.2mm D자 구멍으로 시험 출력 후 실제 축에 맞춥니다.

### 홀 센서 엔코더, CHA, CHB

엔코더 포함 N20은 모터 뒤쪽의 자석과 홀 센서 PCB가 회전을 펄스로 바꿉니다.

```text
자석 회전 → 홀 센서 감지 → CHA/CHB 디지털 펄스 → ESP32
```

- `CHA`, `CHB`는 Wi-Fi/I2C 같은 통신이 아니라 회전 정보를 나타내는 디지털 펄스입니다.
- 두 신호는 약 90도 위상 차이가 있어, 둘 다 읽으면 실제 회전 방향도 알 수 있습니다.
- 현재 v4 기본 코드는 `CHA`만 읽어 속도를 측정합니다. 모터 방향은 TB6612FNG에 내린 명령을 기준으로 판단합니다.
- `7PPR`은 모터 축 기준 기본 펄스 수입니다. 현재 코드처럼 CHA의 상승 에지만 세는 경우, 출력축 1회전당 펄스 수의 시작값은 보통 `7 × 감속비`입니다. 실제 제품마다 다르므로 바퀴를 정확히 한 바퀴 돌려 실측합니다.

### PID 속도 보정

```text
목표 RPM → ESP32 → TB6612FNG PWM → N20 모터
                       ↑                 │
                       └─ 엔코더 펄스 ←──┘
```

모터별 실제 RPM을 계속 측정하고, 목표보다 느린 모터의 PWM은 올리고 빠른 모터의 PWM은 낮춥니다. `Kp`, `Ki`, `Kd`와 모터별 `trim` 값을 조정해 직진 편차를 줄입니다.

## 6. 핀맵

### v0 기본 TT 모터 실습

| 기능 | ESP32 GPIO |
| --- | ---: |
| STBY | 25 |
| 왼쪽 방향 AIN1/AIN2 | 26 / 27 |
| 왼쪽 PWM | 14 |
| 오른쪽 방향 BIN1/BIN2 | 32 / 33 |
| 오른쪽 PWM | 13 |

### v1 차량 VL53L1X

| VL53L1X | 차량 ESP32 |
| --- | ---: |
| SDA | 21 |
| SCL | 22 |
| VIN / GND | 3.3V / GND |

### v2 조이스틱 조종기

| 기능 | 조종기 ESP32 GPIO |
| --- | ---: |
| 조이스틱 VRx | 34 |
| 조이스틱 VRy | 35 |
| 정지 버튼 | 4 |
| 속도 버튼 | 5 |

### v3 BMI270 조종기

| BMI270 | 조종기 ESP32 GPIO |
| --- | ---: |
| SDA / SCL | 21 / 22 |
| INT1 (선택) | 27 |
| VIN / GND | 3.3V / GND |

### v4 차량: 4개 모터와 엔코더

| 위치 | TB6612FNG 채널 | IN1 | IN2 | PWM | 엔코더 CHA |
| --- | --- | ---: | ---: | ---: | ---: |
| 앞왼쪽 FL | #1 A | 18 | 19 | 13 | 34 |
| 앞오른쪽 FR | #1 B | 23 | 25 | 14 | 35 |
| 뒤왼쪽 RL | #2 A | 26 | 27 | 16 | 36 |
| 뒤오른쪽 RR | #2 B | 32 | 33 | 17 | 39 |

두 TB6612FNG의 `STBY`는 ESP32 `3.3V`에 연결해 항상 활성화합니다. GPIO 34/35/36/39는 입력 전용이고 내부 풀업이 없으므로, 엔코더 출력 방식이 오픈 컬렉터이면 CHA와 3.3V 사이에 10kΩ 풀업 저항을 추가합니다.

## 7. 실습 코드 순서

| 순서 | 코드 | 확인할 것 |
| --- | --- | --- |
| 1 | `v0/firmware/01_motor_test` | 기본 모터 방향 |
| 2 | `v1/firmware/01_distance_read` | 거리값 출력 |
| 3 | `v2/firmware/00_print_mac` | 차량·조종기 MAC 주소 |
| 4 | `v2/firmware/controller_joystick` + `vehicle_receiver` | ESP-NOW 주행·통신 끊김 정지 |
| 5 | `v3/firmware/00_bmi270_read` | BMI270 가속도·자이로 값 |
| 6 | `v4/firmware/00_encoder_read` | 엔코더 펄스와 PPR 실측 |
| 7 | `v4/firmware/01_speed_pid_tune` | PID와 모터별 trim 조정 |
| 8 | `v4/firmware/vehicle_encoder_receiver` | 무선 제어·거리 정지·PID 통합 |

## 8. 3D 프린팅과 STL/CAD 관리

현재 저장소에는 외부 STL을 복사해 넣지 않았습니다. 외부 모델은 모터 길이, 엔코더 PCB 폭, 브래킷 홀 위치, 라이선스가 다를 수 있기 때문입니다. 아래 자료는 **참고·다운로드 후보**입니다.

| 참고 자료 | 용도 |
| --- | --- |
| [N20 엔코더 Differential Drive 하드웨어](https://github.com/ggldnl/Differential-Drive-Hardware) | N20 엔코더 모터용 STL/3MF, BOM, 조립 구조 참고 |
| [N20 엔코더 모터 마운트](https://3dgo.app/models/makerworld/2386911) | 엔코더 뒤쪽까지 고려한 브래킷 구조 참고 |
| [N20 4WD 샤시 모델](https://3dgo.app/models/makerworld/2800477) | N20 네 개를 쓰는 4륜 샤시 참고 |
| [BurgerBot N20 모터 홀더 STL](https://www.kevsrobots.com/projects/burgerbot/stl) | 분리형 모터 홀더와 캐스터 설계 참고 |
| [ESP32 로봇 3D 프린팅 FreeCAD 자료](https://github.com/Robotisim/mobile_robotics_3D_printing) | FreeCAD 기반 샤시·ESP32 로봇 설계 참고 |

### 조이스틱 조종기 케이스 참고 자료 (v2/v3)

| 참고 자료 | 용도 |
| --- | --- |
| [Wallieonline ESP-NOW Remote Controller](https://www.wallieonline.nl/blogs/esp-now-remote-control-mini-robots.html) | ESP32 Dev board + 아날로그 조이스틱 + ESP-NOW 조종기 구성 참고 |
| [Wallieonline 조종기 케이스 MakerWorld 파일](https://makerworld.com/en/models/695669#profileId-624636) | 현재 v2와 가장 가까운 조이스틱 하우징 STL 참고 |
| [Wallieonline 조종기 펌웨어](https://github.com/wallieonline/worc-remote-controller-esp32) | 조이스틱 입력을 ESP-NOW로 보내는 코드 구조 참고 |
| [GripCtrl](https://github.com/adrianr3/gripctrl) | ESP32 DevKitC, 조이스틱 2개, 버튼·OLED를 갖춘 확장형 조종기 CAD·회로 참고 |
| [Micro Remote Control](https://www.instructables.com/Micro-Remote-Control/) | 조이스틱 1개와 버튼을 쓰는 소형 3D 프린팅 조종기 참고 |
| [NeoGrip](https://github.com/AthemiS13/NeoGrip) | ESP32·KY-023 조이스틱·버튼을 넣는 인체공학적 손잡이와 STEP 파일 참고 |

현재 RC_CAR에는 **Wallieonline 조종기 케이스를 가장 먼저 참고**합니다. 단, STL을 바로 출력하지 말고 ESP32 DevKit V1, KY-023 조이스틱, 정지·속도 버튼의 실제 길이·폭·높이와 USB 포트 위치를 비교합니다.

```text
controller-case/
├─ top-cover       조이스틱 구멍, 정지·속도 버튼, USB 포트 구멍
├─ inner-deck      ESP32 DevKit V1, KY-023, 버튼 2개 고정
└─ bottom-case     M3 히트셋 인서트와 나사 체결부
```

조종기는 기본 구성에서 USB 케이블 또는 USB 보조배터리로 전원을 공급합니다. 리튬 셀·충전회로는 현재 기본 구조에 포함하지 않습니다.

다운로드·직접 설계한 3D 파일은 아래처럼 보관합니다.

```text
hardware/
├─ reference-models/   외부 참고 링크와 라이선스 메모
├─ cad/                수정 가능한 원본: .FCStd / .step / .scad
├─ stl/                출력용 최종 STL
└─ print-settings/     필라멘트·레이어·인필 설정
```

### 출력할 모듈 순서

1. `motor-bracket` × 4: 실제 JGA12-N20 길이·고정홀에 맞춘 브래킷
2. `wheel-hub` × 4: 3mm D축 시험 허브
3. `base-chassis`: 모터 브래킷과 배터리 홀더를 체결하는 하부 판
4. `electronics-deck`: ESP32, TB6612FNG 2개, 5V 벅-부스트 고정판
5. `front-tof-mount`: VL53L1X 전방 브래킷과 범퍼
6. `top-cover`: 탈착식 상부 커버
7. `controller-case`: v2/v3 조종기 케이스

### 권장 출력 기준

| 부품 | 재료 | 시작 설정 |
| --- | --- | --- |
| 샤시·모터 브래킷 | PETG | 0.2mm 레이어, 벽 4겹, 인필 30~40% |
| 상부 커버 | PLA 또는 PETG | 벽 2~3겹, 인필 15~20% |
| 범퍼 | TPU 선택 | 벽 3겹, 인필 약 20% |

모터 브래킷과 바퀴 허브는 반드시 작은 시험 출력부터 하고, M3 히트셋 인서트와 M3 나사로 교체 가능하게 만듭니다.

## 9. 구매 전 최종 확인 목록

- [ ] 모터 4개가 모두 `6V`, 같은 RPM·감속비 옵션인가?
- [ ] 엔코더에 `VCC`, `GND`, `CHA`, `CHB` 선이 실제로 있는가?
- [ ] 엔코더 출력을 3.3V에서 사용할 수 있는가?
- [ ] 모터 정지전류가 TB6612FNG 채널 허용 전류 안에 있는가?
- [ ] 바퀴 허브가 실제 `3mm D축`과 맞는가?
- [ ] 모터 브래킷의 길이가 엔코더 PCB까지 포함하는가?
- [ ] AA 배터리 4개와 홀더·스위치가 준비됐는가?
- [ ] 5V 2A 이상 벅-부스트 컨버터가 준비됐는가?
- [ ] 두 드라이버용 470uF 커패시터가 준비됐는가?

## 10. 다음 행동

1. N20 모터 상품의 정확한 길이, 감속비, 정지전류, 축 치수를 판매자에게 확인합니다.
2. `motor-bracket`과 `wheel-hub`만 먼저 시험 출력합니다.
3. v4의 `00_encoder_read`를 올려 엔코더 펄스를 측정합니다.
4. `PULSES_PER_WHEEL_REV`를 코드에 입력하고 PID를 튜닝합니다.
5. 치수가 확정되면 샤시와 상부 커버 CAD/STL을 이 저장소의 `hardware/`에 추가합니다.
