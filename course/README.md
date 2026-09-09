# 전체 강의 흐름

한 단계에서 입력·판단·출력을 이해한 뒤, 다음 단계에서 입력 장치나 피드백을 하나씩 확장한다.

| 단계 | 수업에서 설명할 내용 | 핵심 코드 읽기 | 하드웨어 실습 | 학생 결과물 |
|---|---|---|---|---|
| [v0](../v0/README.md) | ESP32가 방향 신호와 PWM을 만들고 드라이버가 모터를 구동하는 과정 | setup/loop, pinMode, digitalWrite, analogWrite, 함수 인자, if | 공통 GND, 좌우 모터 연결과 방향 확인 | 시리얼 f/b/l/r/s 주행 |
| [v1](../v1/README.md) | 거리를 숫자로 읽고 전진 허용 여부를 판단하는 과정 | Wire, 센서 초기화, 조건식, millis, 현재 명령 상태 | SDA/SCL과 센서 전원, 거리별 측정 | 20cm 이하 전진 정지 시험 |
| [v2](../v2/README.md) | 조종기 입력을 명령 패킷으로 바꿔 차량에 전달하는 과정 | analogRead, map, 데드존, struct, callback, 시간 초과 | ESP32 2개, 조이스틱, 정지 버튼 | 무선 조종·통신 끊김 정지 시험 |
| [v3](../v3/README.md) | 가속도로 기울기를 추정하고 같은 패킷으로 보내는 과정 | atan2, sqrt, 평균, 중립 오프셋, 각도→명령 변환 | BMI270 I²C 배선과 장착 방향 | 중립·전후·좌우 제스처 시연 |
| [v4](../v4/README.md) | 모터를 돌리는 명령과 실제 회전 속도의 차이를 보정하는 과정 | ISR, volatile, 임계 구역, 배열, dt, P/I/D | 모터별 독립 구동, CHA 4개, PPR 실측 | 네 RPM 기록·직진 편차 비교 |

## 수업 진행 방식

1. 부품 하나의 역할과 신호를 설명한다.
2. 전원을 끈 상태에서 배선하고 단품 읽기/구동 코드를 업로드한다.
3. 시리얼 출력과 실제 동작을 비교한다.
4. 기존 동작에 새 기능을 연결한다.
5. 실패 상황도 시험하고 결과를 기록한다.

v0의 간단한 주행이 v1의 안전 판단을 이해하는 출발점이다. v2에서는 입력이 PC에서 무선 조종기로 바뀐다. v3는 차량을 바꾸기보다 조종기 입력을 교체하는 실습이다. v4는 차량 구동계와 제어 방식을 바꾸는 단계다.

## 공통 준비

- [설치·업로드](setup/README.md)
- [장비별 설정값](CONFIGURATION.md)
- [부품표](../parts/BOM.md)와 각 버전의 배선표
- [실습 기록](LAB_RECORD.md)
- [샤시 설계](../hardware/README.md): 실제 부품 외곽과 장착홀을 측정한 뒤 진행

## Python과 연결하기

Python은 PC에서 ESP32가 출력하는 USB 시리얼 데이터를 읽는다. 센서 읽기와 차량의 최종 무선 제어는 Arduino 펌웨어가 담당한다.

| 연결 단계 | Python 실습 | 배우는 내용 |
|---|---|---|
| v1 거리 단품 | [02_vl53l1x_serial_lab.py](../python/02_vl53l1x_serial_lab.py) | 숫자 추출, 임계값, 상태 표시, CSV |
| v1 안전 주행 | [04_motor_safety_serial_lab.py](../python/04_motor_safety_serial_lab.py) | 키 입력과 시리얼 명령 송신 |
| v3 센서 단품 | [01_bmi270_serial_lab.py](../python/01_bmi270_serial_lab.py) | 가속도·자이로 로그, 각도 계산, 제스처 판정 |
| v4 엔코더 단품 | [03_encoder_rpm_serial_lab.py](../python/03_encoder_rpm_serial_lab.py) | 네 RPM 비교, 최대·최소 편차, CSV |

Python의 제스처 판정 예제는 원시 가속도로 각도를 계산한다. v3 통합 조종기의 부팅 시 중립 보정과는 구분해서 설명한다.
