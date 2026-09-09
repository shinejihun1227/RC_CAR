# RC_CAR 샤시 먼저 만들기

## 이 파일의 목적

`RC_CAR_Chassis_Frame_Builder.py`는 모터나 전자부품을 얹기 전에 **샤시 프레임 자체의 구조와 크기만 확인하는 첫 단계**입니다.

이 스크립트는 다음 구조만 만듭니다.

```text
RC_CAR_CHASSIS_ONLY
├─ 01_CHASSIS_Base_Plate
├─ 01_CHASSIS_Left_Side_Rail
├─ 01_CHASSIS_Right_Side_Rail
├─ 01_CHASSIS_Front_Bumper
├─ 01_CHASSIS_Rear_Bumper
├─ 02_CHASSIS_Mounting_Bridge_Front
├─ 02_CHASSIS_Mounting_Bridge_Center
├─ 02_CHASSIS_Mounting_Bridge_Rear
├─ 03_CHASSIS_Deck_Post_FL/FR/RL/RR
└─ 99_CHASSIS_Rigid_Structure
```

모터, 바퀴, ESP32, TB6612FNG, 배터리, 센서, 저장소의 STL 파일은 이 단계에서 생성하거나 삽입하지 않습니다. 따라서 화면에서 무엇이 샤시인지 바로 구분할 수 있습니다.

## 기본 설계값

| 항목 | 기본값 | 역할 |
|---|---:|---|
| 전체 폭 × 길이 | 300 × 400 mm | 대형 RC카 기준 외곽 |
| 하부 플레이트 | 5 mm | 주 구조판 |
| 중앙 개구부 | 140 × 240 mm | 무게 감소 및 배선 공간 |
| 좌우 레일 | 8 × 380 × 24 mm | 비틀림 보강 및 측면 결속 |
| 앞뒤 범퍼 | 270 × 12 × 28 mm | 끝단 보호 및 가로 보강 |
| 장착 브리지 | 3개, 폭 260 mm | 이후 N20/TT 브래킷을 고정할 구조 |
| 상부 데크 포스트 | Ø10 × 22 mm, 4개 | 나중에 상부 프레임을 올릴 지지점 |
| 기본 나사 구멍 | Ø3.4 mm | M3 관통 여유홀 시작값 |

실제 출력에서는 소재, 프린터, 나사 종류에 따라 구멍 여유를 시험해야 합니다. 위 값은 완성품 치수를 보장하는 값이 아니라 첫 설계 검토용 기준값입니다.

## Fusion 360에서 실행하는 순서

1. `Utilities → Add-Ins → Scripts and Add-Ins`를 엽니다.
2. `My Scripts`에서 `RC_CAR_Chassis_Frame_Builder.py`를 선택합니다.
3. `Run`을 누릅니다.
4. Browser에서 `RC_CAR_CHASSIS_ONLY` 아래 구조를 확인합니다.
5. `Modify → Change Parameters`에서 `chassis_width`, `chassis_length`, `central_opening_width` 등을 바꿔봅니다.

이번 단계에서는 기존의 `RC_CAR_Mounting_Frame_Builder.py`를 실행하지 않습니다. 그 파일은 이후 모터·전자부품 장착 위치까지 검토하는 조립 단계용입니다.

## 왜 이런 구조로 시작했는가

- [Motor Mash](https://github.com/rbricheno/motor-mash)는 N20을 TT 샤시 계열로 확장하는 측면 클램프, 축 중심, 배선 통로를 보여줍니다. 그래서 이 모델은 모터 형상을 샤시에 박아 넣지 않고, 나중에 교체 가능한 브리지와 브래킷을 받을 수 있게 했습니다.
- [Cubie-3 STL 구조](https://www.kevsrobots.com/projects/cubie-3/stl)는 별도 베이스, 상부 선반, 포스트, 모터 홀더를 분리합니다. 그래서 하부 샤시와 상부 프레임을 처음부터 하나의 덩어리로 만들지 않았습니다.
- [TT motor mounting](https://github.com/grimmpp/tt-motor-mounting)은 TT 모터용 브리지와 베이스를 별도 부품으로 나누는 방식을 참고했습니다. 이 프로젝트의 형상을 복사한 것이 아니라, 교체 가능한 결속부라는 원칙만 반영했습니다.

스크립트는 위 예시의 메시/STL을 그대로 가져오지 않고, Fusion 360의 스케치 → 돌출 → 절삭 → 필렛 순서로 샤시 구조를 새로 만듭니다. 따라서 이후 파라미터를 바꾸거나 피처를 직접 수정할 수 있습니다.

## 처음부터 전체 출력하지 않기

300 × 400 mm는 큰 프레임이므로 220 × 220 mm급 프린터라면 전체 출력 전에 다음만 먼저 확인하는 것이 안전합니다.

1. 장착 브리지 1개를 짧은 시편으로 출력합니다.
2. M3 나사가 통과하는지 확인합니다.
3. 프레임 재료와 두께를 결정합니다.
4. 그다음 전체 샤시를 분할하거나 가공 가능한 판재로 제작합니다.

N20과 TT 모터의 실제 고정홀 위치가 확정되면, 다음 단계에서 이 브리지 위에 **별도 모터 브래킷 컴포넌트**를 추가합니다. 샤시 파일을 먼저 고정해 두면 모터 모델 때문에 기본 프레임이 계속 흔들리는 일을 줄일 수 있습니다.
