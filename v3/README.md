# v3 · BMI270 손 제스처 조종

손 조종기의 기울기를 전진·후진·조향 명령으로 변환한다.

## 배우는 코딩

Arduino IDE에서 C 문법 중심으로 진행한다. [단계별 문법·수정 과제](../course/ARDUINO_C_GUIDE.md)를 함께 사용한다.

단품 실습에서는 가속도와 자이로를 모두 읽는다. 현재 조종기의 Roll/Pitch 계산은 가속도에 atan2·sqrt를 적용한다. 부팅 시 100회 평균으로 중립을 보정하고, 8° 미만 데드존과 35° 최대 명령 범위를 적용한다. 자이로 융합·절대 Yaw 제어는 기본 구현에 없다.

## 하드웨어와 부품

v2의 조종기 ESP32를 재사용하고 BMI270 1개를 추가한다. SDA/SCL은 GPIO 21/22, 정지 버튼은 GPIO 4다. ESP32를 세 번째로 추가하지 않는다.

[배선표](docs/wiring.md) · [단계별 부품표](docs/bom.md) · [구매 규격](../parts/purchasing/v3.md) · [부품 원리](../parts/PRINCIPLES.md)

## 실습 순서

[설치 안내](../course/setup/README.md)를 확인하고 표의 순서대로 각 스케치를 별도로 업로드한다.

| 스케치 | 실습 내용 |
|---|---|
| [00_bmi270_read](firmware/00_bmi270_read/00_bmi270_read.ino) | 가속도 g·각속도 dps 출력 |
| [01_gesture_map_test](firmware/01_gesture_map_test/01_gesture_map_test.ino) | Roll/Pitch와 STOP/FORWARD/BACKWARD/LEFT/RIGHT 비교 |
| [controller_bmi270](firmware/controller_bmi270/controller_bmi270.ino) | 중립 보정 후 기울기를 ESP-NOW 명령으로 송신 |

차량에는 [v2 vehicle_receiver](../v2/firmware/vehicle_receiver/vehicle_receiver.ino)를 업로드한다. [제스처 맵](docs/gesture-map.md)에서 단품 판정과 통합 조종기의 차이를 확인한다.

## 확인할 결과

부팅 중 조종기를 고정하고, 중립·기울기·정지 버튼·통신 끊김을 시험한다. 센서 장착 방향에 따라 각도 부호를 실제 동작과 비교한다.

[요구사항](docs/requirements.md) · [시험 절차](docs/test-plan.md) · [장비 설정](../course/CONFIGURATION.md) · [실습 기록](../course/LAB_RECORD.md)

## 다음 단계

v4에서 차량을 엔코더 N20 4륜 구동으로 교체한다.

[전체 학습 흐름](../course/README.md)
