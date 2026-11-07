# RC카 단계별 실습

차량은 [원본 RC 차체 모델](https://makerworld.com/ko/models/1342319-rc-rover-with-robot-arm-6-dof#profileId-1383072)을 바탕으로 Arduino Mega 2560·L298N·DC 모터 2개·VL53L0X 거리센서를 사용한다. 조종기는 Arduino Uno·nRF24L01+ 무선 모듈·조이스틱 2개를 사용하고, v4에서 KCD1 스위치와 BMI270을 더한다. 로봇팔은 이번 범위에 포함하지 않는다.

**[v0~v4 업로드·배선·확인 순서](docs/v0-v4-practice.md)**를 먼저 읽는다. 각 버전의 `.ino` 파일은 Arduino IDE에서 직접 열 수 있다. 이전에 작성한 `now_...` 파일과 기존 문서는 참고용으로 남겨 두었고, 아래 표가 현재 실습 순서다.

| 버전 | 추가되는 기능 | 차량 Mega에 업로드 | 조종기 Uno에 업로드 | 통과 기준 |
|---|---|---|---|---|
| v0 | 모터·L298N | [vehicle_mega](firmware/v0/vehicle_mega/vehicle_mega.ino) | 없음 | 좌우 모터가 각각 전·후진한다. |
| v1 | VL53L0X 거리센서 | [vehicle_mega](firmware/v1/vehicle_mega/vehicle_mega.ino) | 없음 | 거리 값이 바뀌고 30 cm 이내에서 전진이 막힌다. |
| v2 | 무선 송수신, 시리얼 조종 | [vehicle_mega](firmware/v2/vehicle_mega/vehicle_mega.ino) | [controller_uno](firmware/v2/controller_uno/controller_uno.ino) | Uno의 시리얼 명령이 차량에 전달되고 `ACK=YES`가 나온다. |
| v3 | 조이스틱 2개로 주행 | [vehicle_mega](firmware/v3/vehicle_mega/vehicle_mega.ino) | [controller_uno](firmware/v3/controller_uno/controller_uno.ino) | 왼쪽 조이스틱 전후진, 오른쪽 조이스틱 좌우 조향. |
| v4 | KCD1 모드 선택·BMI270 기울기 | [vehicle_mega](firmware/v4/vehicle_mega/vehicle_mega.ino) | [controller_uno](firmware/v4/controller_uno/controller_uno.ino) | 스위치를 바꾸면 조이스틱/기울기 모드가 전환된다. |

v2~v4의 차량 코드는 의도적으로 같다. 차량의 무선 수신·거리 제한은 그대로 두고 조종기 입력을 시리얼 → 조이스틱 → 조이스틱/기울기 모드로 늘린다. 모든 버전은 앞 단계를 통과한 뒤 진행한다. **현재 왼쪽 OUT1·OUT2 채널은 실물 점검 중이므로 v0 성공 전 차량 주행을 시작하지 않는다.**

Arduino IDE 라이브러리: v1부터 **VL53L0X (Pololu)**, v2부터 **RF24 (TMRh20/nRF24)**, v4부터 **[NiusIMU 0.4.1](https://github.com/dunknowcoding/ArduinoNRF-IMU/tree/v0.4.1)**. BMI270 기판의 전원·I²C 허용 전압은 실물 사양 확인이 필요하다. 코드는 컴파일 검증과 실물 검증을 구분해 기록한다.

2026-11-07 기준 **8개 스케치 모두 Arduino AVR 보드 대상으로 컴파일 통과**. v4 Uno는 프로그램 25,780/32,256 B(79%), 전역 메모리 844/2,048 B(41%)를 사용했다. 실물 주행은 아직 검증하지 않았고, 왼쪽 OUT1·OUT2 출력 문제도 해결 여부를 확인해야 한다.
