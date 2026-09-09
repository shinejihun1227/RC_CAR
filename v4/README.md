# v4 · 엔코더 N20 4륜 PID 제어

네 모터의 회전 속도를 각각 측정하고 목표 RPM과의 차이를 PWM으로 보정한다.

## 배우는 코딩

Arduino IDE에서 C 문법 중심으로 진행한다. [단계별 문법·수정 과제](../course/ARDUINO_C_GUIDE.md)를 함께 사용한다.

인터럽트에서 CHA 상승 에지를 센다. 임계 구역으로 카운터를 복사하고 펄스 수·측정 시간·PPR로 RPM을 계산한다. 배열과 반복문으로 모터 네 개를 처리하며 P/I/D와 trim으로 출력을 보정한다. 엔코더는 CHA만 읽으므로 회전 방향을 직접 측정하지 않는다.

## 하드웨어와 부품

엔코더 N20 4개, TB6612FNG 총 2개, 바퀴·브래킷 각 4개. VL53L1X와 조종기는 재사용한다. v0 핀맵을 그대로 쓰지 않는다. GPIO 34/35/36/39는 엔코더 입력이며 필요한 경우 외부 풀업을 사용한다.

[배선표](docs/wiring.md) · [단계별 부품표](docs/bom.md) · [구매 규격](../parts/purchasing/v4.md) · [부품 원리](../parts/PRINCIPLES.md)

## 실습 순서

[설치 안내](../course/setup/README.md)를 확인하고 표의 순서대로 각 스케치를 별도로 업로드한다.

| 스케치 | 실습 내용 |
|---|---|
| [00_encoder_read](firmware/00_encoder_read/00_encoder_read.ino) | 바퀴별 펄스·RPM, 출력축 PPR 실측 |
| [01_speed_pid_tune](firmware/01_speed_pid_tune/01_speed_pid_tune.ino) | f/b/s와 +/-로 목표 RPM·PID 시험 |
| [vehicle_encoder_receiver](firmware/vehicle_encoder_receiver/vehicle_encoder_receiver.ino) | v2 또는 v3 조종기·전진 거리 정지·4륜 PID 통합 |

[PID 튜닝 순서](docs/pid-tuning.md)를 따르고, 튜닝 코드에서 확인한 PPR·PID·trim 값을 통합 수신기에도 반영한다.

## 확인할 결과

세 스케치의 PPR 기본값 1.0을 실측값으로 바꾼다. 바퀴를 띄워 방향·RPM을 확인하고 저속 직진 편차를 비교한다. PID·trim 기본값은 실물 튜닝 완료값이 아니다.

[요구사항](docs/requirements.md) · [시험 절차](docs/test-plan.md) · [장비 설정](../course/CONFIGURATION.md) · [실습 기록](../course/LAB_RECORD.md)

## 다음 단계

CHB 방향 측정, 속도 기록, 주행 편차 분석을 선택 확장으로 진행한다.

[전체 학습 흐름](../course/README.md)
