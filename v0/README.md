# v0 · 기본 모터 주행

ESP32가 좌우 모터의 방향과 속도를 제어하고 시리얼 명령으로 차를 움직인다.

## 배우는 코딩

상수로 핀과 속도를 정의한다. 함수를 이용해 모터 한 개의 동작을 묶고, if 조건문으로 명령을 해석한다. GPIO 디지털 출력은 방향을, PWM은 구동 세기를 결정한다.

## 하드웨어와 부품

ESP32 1개, TB6612FNG 1개, TT 모터·바퀴 각 2개, AA 4개, 5V 벅-부스트. 모터는 ESP32 GPIO가 아닌 드라이버 출력에 연결한다.

[배선표](docs/wiring.md) · [단계별 부품표](docs/bom.md) · [구매 규격](../parts/purchasing/v0.md) · [부품 원리](../parts/PRINCIPLES.md)

## 실습 순서

[설치 안내](../course/setup/README.md)를 확인하고 표의 순서대로 각 스케치를 별도로 업로드한다.

| 스케치 | 실습 내용 |
|---|---|
| [01_motor_test](firmware/01_motor_test/01_motor_test.ino) | 좌우 모터를 순서대로 구동해 방향 확인 |
| [02_basic_drive](firmware/02_basic_drive/02_basic_drive.ino) | f/b/l/r/s 시리얼 명령으로 전후진·회전·정지 |

## 확인할 결과

전원 투입 때 임의로 움직이지 않고 다섯 명령이 의도대로 동작하는지 확인한다.

[요구사항](docs/requirements.md) · [시험 절차](docs/test-plan.md) · [장비 설정](../course/CONFIGURATION.md) · [실습 기록](../course/LAB_RECORD.md)

## 다음 단계

v1에서 차량 앞의 거리를 읽고 전진 허용 여부를 판단한다.

[전체 학습 흐름](../course/README.md)
