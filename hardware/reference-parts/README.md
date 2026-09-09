# RC_CAR Fusion 360 참조 부품 묶음

이 폴더는 4륜 N20 엔코더 RC카 샤시를 설계할 때 부품을 Fusion 360 화면에 함께 띄워 놓기 위한 **배치·간섭 확인용 참조 모델**입니다.

## 포함 파일

| 파일 | 용도 | 기준 외형 |
|---|---|---:|
| `N20_encoder_motor_reference.stl` | N20 엔코더 모터 외곽 | 본체 약 32.5 × 12 × 10 mm, 축 Ø3 × 9 mm |
| `wheel_70mm_reference.stl` | N20 호환 바퀴 외곽 | Ø70 × 22 mm, 보어 약 Ø3.2 mm |
| `N20_motor_bracket_reference.stl` | 교체형 모터 브래킷 외곽 | 약 40 × 18 × 27.5 mm |
| `ESP32_DevKit_V1_reference.stl` | ESP32 보드 외곽 | 약 55 × 28 × 12 mm |
| `TB6612FNG_breakout_reference.stl` | 모터 드라이버 보드 외곽 | 약 33 × 25 × 8 mm |
| `AA_4cell_holder_reference.stl` | 4칸 AA 배터리 홀더 외곽 | 약 60 × 32 × 31 mm |
| `buck_converter_reference.stl` | 5V 벅-부스트 컨버터 외곽 | 약 45 × 20 × 10 mm |
| `VL53L1X_reference.stl` | 전방 ToF 센서 외곽 | 약 21 × 17 × 6 mm |
| `M3_spacer_10mm_reference.stl` | 상부 데크 지지대 참조 | 외경 약 Ø6.4 × 10 mm |
| `RC_CAR_Reference_Assembly_Builder.py` | Fusion 360에서 참조 조립 배치를 자동 생성 | 샤시 + 주요 부품 외곽 |

## Fusion 360에서 가장 빠르게 쓰는 방법

### 방법 A: 전체 배치 자동 생성

1. Fusion 360에서 `Utilities → Scripts and Add-Ins`를 엽니다.
2. `+` 또는 `Add Existing Script`로 `RC_CAR_Reference_Assembly_Builder.py`를 등록합니다.
3. `Run`을 누릅니다.
4. `RC_CAR_REFERENCE_ASSEMBLY` 아래에 샤시와 부품 외곽이 생성됩니다.
5. 실제 모델링은 참조 부품을 기준으로 진행하고, 필요하지 않은 부품은 Browser의 전구를 끕니다.

### 방법 B: 필요한 STL만 직접 삽입

1. `Insert → Insert Mesh`를 선택합니다.
2. 원하는 `.stl` 파일을 선택합니다.
3. 단위를 `Millimeter`로 지정합니다.
4. `Move/Copy`와 `Align`으로 샤시에 배치합니다.

## 현재 RC_CAR 샤시에 들어갈 주요 부품

- N20 엔코더 기어드모터 × 4
- N20 호환 바퀴 × 4
- N20 교체형 모터 브래킷 × 4
- ESP32 DevKit V1 × 1
- TB6612FNG 모터 드라이버 × 2
- 4칸 AA 배터리 홀더 × 1
- 5V 2A 이상 벅-부스트 컨버터 × 1
- VL53L1X ToF 거리 센서 × 1
- M3 스페이서·나사·인서트
- 전원 스위치, 커패시터, 배선과 커넥터

## 반드시 확인할 점

이 모델들은 특정 판매자의 제품 CAD를 복제한 것이 아니라, 현재 저장소의 치수 기준으로 만든 **참조 외곽 모델**입니다. 실제 출력 전에는 다음을 캘리퍼스로 확인해야 합니다.

- N20 모터의 엔코더 PCB까지 포함한 전체 길이
- 브래킷 고정홀 간격과 축 중심 높이
- 실제 바퀴 보어와 D축 형상
- ESP32 USB 포트 방향
- TB6612FNG 보드 홀 위치
- 4×AA 홀더의 외곽과 스위치 위치

FDM 출력에서는 체결 여유를 우선 `0.4 mm`부터 시험하고, 실제 프린터와 재료에 맞춰 조정하세요. 최종 제작용 치수로 바로 사용하지 마세요.

기술 기준: `course/README.md`, `parts/BOM.md`, `hardware/fusion360/DIMENSION_SOURCES.md`.
