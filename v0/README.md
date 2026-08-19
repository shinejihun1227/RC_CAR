# v0 — 기본 RC카

이 버전의 목표는 센서 없이 RC카를 안정적으로 움직이는 것입니다.

## 완료 기준

- ESP32가 TB6612FNG를 제어한다.
- 좌·우 DC 기어드모터로 전진, 후진, 좌회전, 우회전, 정지한다.
- 전원을 켰을 때 모터가 임의로 움직이지 않는다.

## 진행 순서

1. [요구사항](docs/requirements.md), [블록 다이어그램](docs/block-diagram.md), [부품표](docs/bom.md)를 읽는다.
2. [배선표](docs/wiring.md)대로 연결한다.
3. `firmware/01_motor_test`로 양쪽 모터 방향을 확인한다.
4. `firmware/02_basic_drive`를 업로드하고 시리얼 모니터에서 `f`, `b`, `l`, `r`, `s` 명령을 보낸다.
5. 실습 결과를 Commit으로 남긴다.

## 폴더 구조

```text
docs/       v0 요구사항, 부품, 배선, 조사 기록
firmware/   v0 ESP32 스케치
hardware/   v0 회로 이미지와 실물 사진
```
