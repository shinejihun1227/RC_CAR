# 전체 강의 흐름

Arduino IDE에서 C언어 문법 중심으로 코딩하고 ESP32에서 실행한다. 한 단계에서 입력·판단·출력을 이해한 뒤, 다음 단계에서 입력 장치나 피드백을 하나씩 확장한다. [C 문법·수정 과제 안내](ARDUINO_C_GUIDE.md)를 함께 사용한다.

| 단계 | 수업에서 설명할 내용 | 핵심 코드 읽기 | 하드웨어 실습 | 학생 결과물 |
|---|---|---|---|---|
| [v0](../v0/README.md) | ESP32가 방향 신호와 PWM을 만들고 드라이버가 모터를 구동하는 과정 | int, const, 함수·인자, if, switch, setup/loop, GPIO·PWM | 공통 GND, 좌우 모터 연결과 방향 확인 | 시리얼 f/b/l/r/s 주행 |
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

## Arduino 시리얼 모니터로 확인하기

아래 실습은 Arduino IDE의 시리얼 모니터를 115200 baud로 열어 진행한다. 별도 PC 프로그램 설치 없이 센서 값과 명령을 확인한다.

| 연결 단계 | 업로드할 Arduino 스케치 | 확인할 내용 |
|---|---|---|
| v1 거리 단품 | [01_distance_read](../v1/firmware/01_distance_read/01_distance_read.ino) | distance_mm 값과 실제 거리 비교 |
| v1 안전 주행 | [02_safety_stop](../v1/firmware/02_safety_stop/02_safety_stop.ino) | 입력창에서 f/b/l/r/s를 전송하고 전진 정지 확인 |
| v3 센서 단품 | [00_bmi270_read](../v3/firmware/00_bmi270_read/00_bmi270_read.ino) | 가속도·자이로 값의 축과 단위 |
| v3 제스처 판정 | [01_gesture_map_test](../v3/firmware/01_gesture_map_test/01_gesture_map_test.ino) | Roll/Pitch와 STOP/FORWARD/BACKWARD/LEFT/RIGHT |
| v4 엔코더 단품 | [00_encoder_read](../v4/firmware/00_encoder_read/00_encoder_read.ino) | 네 바퀴의 펄스 수와 RPM 기록 |
| v4 PID 조정 | [01_speed_pid_tune](../v4/firmware/01_speed_pid_tune/01_speed_pid_tune.ino) | f/b/s와 +/- 명령으로 목표 RPM과 실제 RPM 비교 |

v3 제스처 판정 단품은 원시 가속도로 각도를 계산한다. v3 통합 조종기는 부팅 시 100회 평균으로 중립을 보정하므로 둘의 출력 기준을 구분해서 설명한다. 측정값은 [실습 기록](LAB_RECORD.md)에 적는다.
