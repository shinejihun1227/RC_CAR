# RC_CAR

ESP32로 만드는 2WD Wi-Fi RC카 프로젝트입니다. 이 저장소는 완성된 예제를 단순히 복사하는 대신, GitHub에서 프로젝트를 찾고 문서·회로·코드를 분석한 뒤 기능을 확장하는 실습형 강의의 기준 저장소입니다.

## 첫 번째 목표: RC Car v0

- ESP32가 TB6612FNG 모터 드라이버를 제어한다.
- 좌·우 DC 기어드모터로 전진, 후진, 좌회전, 우회전, 정지한다.
- 센서는 v0에 넣지 않는다. 주행 기반이 안정화된 뒤 확장한다.

## 시작하기

1. GitHub에서 `RC_CAR shinejihun1227`를 검색해 이 저장소를 찾습니다.
2. [요구사항](docs/requirements.md), [블록 다이어그램](docs/block-diagram.md), [부품표](docs/bom.md)를 읽습니다.
3. `firmware/01_motor_test`로 모터 방향을 먼저 확인합니다.
4. `firmware/02_basic_drive`로 기본 주행 동작을 확인합니다.
5. 실습 결과와 발견한 문제를 Issue와 Commit으로 남깁니다.

## 저장소 구조

```text
docs/       요구사항, 조사 기록, 회로·배선 문서
firmware/   ESP32 Arduino 스케치
hardware/   회로 이미지와 실물 사진을 보관할 폴더
```

## 단계별 확장 계획

| 버전 | 기능 | 권장 작업 방식 |
| --- | --- | --- |
| v0.1 | 모터 단품 테스트 | `feature/motor-test` 브랜치 |
| v0.2 | 기본 주행 | `feature/basic-drive` 브랜치 |
| v0.3 | Wi-Fi 웹 조종 | `feature/web-control` 브랜치 |
| v1.0 | FSR406 압력센서 | Issue 생성 후 기능 브랜치 |
| v1.1 | SHTC3 및 I2C MUX | Issue 생성 후 기능 브랜치 |
| v1.2 | 진동모터·레이저 피드백 | Issue 생성 후 기능 브랜치 |

## 라이선스와 안전

배터리와 모터를 연결할 때는 전원을 끈 상태에서 배선하고, ESP32와 모터 드라이버의 GND를 공통으로 연결합니다. 레이저는 사람이나 반사면을 향하지 않도록 저출력 모듈만 사용합니다.
