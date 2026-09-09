# RC_CAR 로버형 샤시

MakerWorld의 [RC Rover with Robot Arm 6 DOF](https://makerworld.com/ko/models/1342319-rc-rover-with-robot-arm-6-dof#profileId-1383072)의 전체 STL을 복사한 파일이 아닙니다. 해당 모델에서 보이는 로버형 레이어 구조를 참고하여, 현재 프로젝트의 N20/TT 공통 장착 방향에 맞춘 Fusion 360 네이티브 파라미터 모델입니다.

## 실행 파일

`RC_CAR_Rover_Chassis_Builder.py`

Fusion 360에서 `Utilities → Add-Ins → Scripts and Add-Ins`를 열고 이 파일을 선택한 뒤 `Run`을 누릅니다.

## 생성되는 구조

```text
RC_CAR_ROVER_CHASSIS
├─ 01_ROVER_Lower_Tub_Plate
├─ 01_ROVER_Left/Right_Raised_Side_Rail
├─ 01_ROVER_Front/Rear_Bumper_Beam
├─ 02_ROVER_Motor_Interface_Pad_FL/FR/RL/RR
├─ 03_ROVER_Upper_Deck_Post_FL/FR/RL/RR
├─ 03_ROVER_Upper_Electronics_Deck
└─ 04_ROVER_Arm_Base_Interface
```

모터나 바퀴를 모델 안에 집어넣지 않았습니다. 대신 네 모서리의 `Motor_Interface_Pad`에 나중에 N20 브래킷 또는 TT 브래킷을 M3 나사로 교체 장착하는 방식입니다. 즉, 이 파일에서 먼저 확인할 것은 차체의 비율과 체결 공간입니다.

## 기본 크기

| 항목 | 값 |
|---|---:|
| 하부 차체 | 320 × 420 × 6 mm |
| 하부 서비스 개구부 | 150 × 260 mm |
| 좌우 레일 | 8 × 370 × 36 mm |
| 앞뒤 범퍼 | 290 × 12 × 32 mm |
| 모터 어댑터 패드 | 82 × 28 × 8 mm, 4개 |
| 패드 M3 홀 패턴 | 50 × 14 mm 중심 간격 |
| 상부 데크 | 280 × 250 × 5 mm |
| 작업 장치 패드 | Ø126 × 6 mm |

320 × 420 mm는 링크 모델처럼 큰 로버 느낌을 내기 위한 기준입니다. 220 × 220 mm급 프린터로 한 번에 출력하기에는 크므로, 실제 제작은 알루미늄/아크릴 판재로 가공하거나 하부 차체를 2~4개로 분할하는 방식을 권장합니다.

## 링크 모델에서 참고한 부분

링크의 설명에는 차체와 6축 암을 통합한 구조, 37 mm 감속 모터, 608 베어링, M8 축, 여러 길이의 M3 체결재가 제시되어 있습니다. 이 모델의 특징을 그대로 복제하지 않고 다음 설계 원칙만 반영했습니다.

- 하부 차체와 상부 데크를 분리
- 좌우 구동부를 수용할 높은 측면 레일
- 상부 중앙의 작업 장치 장착 인터페이스
- 정비·배선을 위한 중앙 개구부
- 모터를 직접 고정하지 않고 교체 가능한 어댑터 패드 사용

원본 MakerWorld 페이지의 라이선스는 `Standard Digital File License`로 표시되므로, 원본 STL을 프로젝트에 복사하지 않고 구조적 아이디어만 참고했습니다. [원본 모델](https://makerworld.com/ko/models/1342319-rc-rover-with-robot-arm-6-dof#profileId-1383072)과 [모델 정보 미러](https://www.3dgo.app/models/makerworld/1342319)

## 변경할 때 주의할 점

`Modify → Change Parameters`에서 판 두께와 데크 두께는 먼저 조정할 수 있습니다. 전체 폭·길이를 크게 바꾸거나 포스트 위치를 변경할 때는 파라미터를 바꾼 뒤 스크립트를 다시 실행하는 편이 안전합니다. 발생 위치가 발생학적으로 모두 연결된 단일 스케치가 아니라, 읽기 쉬운 부품별 컴포넌트로 나뉘어 있기 때문입니다.

N20/TT 호환은 네 개의 패드에 실제 브래킷을 고정하는 설계입니다. 모터의 실제 고정홀, 축 중심 높이, 바퀴 지름을 확인하기 전에는 패드에 모터 형상을 직접 결합하지 않습니다.
