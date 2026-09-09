# v1 · 거리 감지와 안전 정지

VL53L1X 거리값을 읽고 장애물이 가까워지면 전진을 차단한다.

## 배우는 코딩

Wire와 센서 라이브러리로 I²C 장치를 초기화한다. mm 단위 값을 임계값과 비교한다. 현재 명령을 저장하고 loop에서 센서를 반복 감시하여 키 입력이 없어도 상태를 갱신한다.

## 하드웨어와 부품

v0 차량 + VL53L1X 1개. 센서 SDA는 GPIO 21, SCL은 GPIO 22, 전원은 3.3V 호환 모듈 기준이다.

[배선표](docs/wiring.md) · [단계별 부품표](docs/bom.md) · [구매 규격](../parts/purchasing/v1.md) · [부품 원리](../parts/PRINCIPLES.md)

## 실습 순서

[설치 안내](../course/setup/README.md)를 확인하고 표의 순서대로 각 스케치를 별도로 업로드한다.

| 스케치 | 실습 내용 |
|---|---|
| [01_distance_read](firmware/01_distance_read/01_distance_read.ino) | 단독 거리 출력과 측정 실패 확인 |
| [02_safety_stop](firmware/02_safety_stop/02_safety_stop.ino) | 시리얼 주행에 반복 거리 감시와 전진 정지 결합 |

## 확인할 결과

f를 한 번만 입력한 뒤 장애물을 접근시켜도 멈춰야 한다. 200mm 이하와 측정 불가 시 전진을 차단한다. 500mm 이내 경고는 출력만 하며 자동 감속은 없다. 장애물 정지 후에는 새로운 f 명령으로 재출발한다.

[요구사항](docs/requirements.md) · [시험 절차](docs/test-plan.md) · [장비 설정](../course/CONFIGURATION.md) · [실습 기록](../course/LAB_RECORD.md)

## 다음 단계

v2에서 PC 명령 대신 조이스틱 무선 명령을 받는다.

[전체 학습 흐름](../course/README.md)
