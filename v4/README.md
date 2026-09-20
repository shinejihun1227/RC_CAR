# v4 · 엔코더 N20 2구동륜 PID 제어

좌우 N20 엔코더 모터 2개의 회전 속도를 측정하고 목표 RPM과의 차이를 PWM으로 보정한다. 뒤쪽 2바퀴는 자유회전 수동 바퀴다.

## 배우는 코딩

Arduino IDE에서 C 문법 중심으로 진행한다. [단계별 문법·수정 과제](../course/ARDUINO_C_GUIDE.md)를 함께 사용한다.

인터럽트에서 좌우 CHA 상승 에지를 센다. 임계 구역으로 카운터를 복사하고 펄스 수·측정 시간·PPR로 RPM을 계산한다. 배열과 반복문으로 구동 모터 2개를 처리하며 P/I/D와 trim으로 출력을 보정한다. 수동 바퀴에는 모터나 엔코더를 연결하지 않는다. 엔코더는 CHA만 읽으므로 회전 방향을 직접 측정하지 않는다.

## 하드웨어와 부품

엔코더 N20 2개, 자유회전 수동 바퀴 2개, N20 브래킷 2개, TB6612FNG 1개를 사용한다. VL53L1X와 조종기는 재사용한다. v0 핀맵을 그대로 쓰지 않는다. GPIO 34/35는 엔코더 입력이며 필요한 경우 외부 풀업을 사용한다.

[배선표](docs/wiring.md) · [전원 시스템 검토](docs/power-analysis.md) · [단계별 부품표](docs/bom.md) · [구매 규격](../parts/purchasing/v4.md) · [부품 원리](../parts/PRINCIPLES.md)

차체와 모터 마운트는 [실측 부품 기반 Fusion 360 설계 안내](../hardware/fusion360/MEASURED_PARTS_WORKFLOW.md)에 따라 부품 외곽·축·장착홀을 확인하고 한 개를 시험 출력한 뒤 제작한다.

## 실습 순서

[설치 안내](../course/setup/README.md)를 확인하고 표의 순서대로 각 스케치를 별도로 업로드한다.

| 스케치 | 실습 내용 |
|---|---|
| [00_encoder_read](firmware/00_encoder_read/00_encoder_read.ino) | 좌우 구동륜 펄스·RPM, 출력축 PPR 실측 |
| [01_speed_pid_tune](firmware/01_speed_pid_tune/01_speed_pid_tune.ino) | f/b/s와 +/-로 목표 RPM·PID 시험 |
| [vehicle_encoder_receiver](firmware/vehicle_encoder_receiver/vehicle_encoder_receiver.ino) | v2 또는 v3 조종기·전진 거리 정지·좌우 2모터 PID 통합 |

[PID 튜닝 순서](docs/pid-tuning.md)를 따르고, 튜닝 코드에서 확인한 PPR·PID·trim 값을 통합 수신기에도 반영한다.

## 확인할 결과

세 스케치의 PPR 기본값 1.0을 좌우 구동륜 실측값으로 바꾼다. 바퀴를 띄워 방향·RPM을 확인하고 저속 직진 편차를 비교한다. PID·trim 기본값은 실물 튜닝 완료값이 아니다. 수동 바퀴가 바닥에 고르게 닿고 구동 바퀴와 같은 접지 높이를 갖는지 별도로 확인한다.

[요구사항](docs/requirements.md) · [시험 절차](docs/test-plan.md) · [장비 설정](../course/CONFIGURATION.md) · [실습 기록](../course/LAB_RECORD.md)

## 다음 단계

CHB 방향 측정, 속도 기록, 주행 편차 분석을 선택 확장으로 진행한다.

[전체 학습 흐름](../course/README.md)
