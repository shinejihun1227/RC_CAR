# v2 · 조이스틱 무선 조종

별도 ESP32 조종기에서 차량으로 주행 명령을 전송한다.

## 배우는 코딩

Arduino IDE에서 C 문법 중심으로 진행한다. [단계별 문법·수정 과제](../course/ARDUINO_C_GUIDE.md)를 함께 사용한다.

ADC 값 0~4095를 전후진·조향 명령 -255~255로 변환한다. 데드존으로 중립 흔들림을 줄인다. struct에 명령을 묶어 ESP-NOW로 보내고, 수신 콜백과 loop 사이에서 최근 명령을 공유한다. 수신 시간 초과를 millis로 판단한다.

## 하드웨어와 부품

차량과 조종기에 ESP32 각 1개. 조이스틱 VRx/VRy는 조종기 GPIO 34/35, 정지 버튼은 GPIO 4. 기본 코드에는 별도 속도 모드 버튼이 없다.

[배선표](docs/wiring.md) · [단계별 부품표](docs/bom.md) · [구매 규격](../parts/purchasing/v2.md) · [부품 원리](../parts/PRINCIPLES.md)

## 실습 순서

[설치 안내](../course/setup/README.md)를 확인하고 표의 순서대로 각 스케치를 별도로 업로드한다.

| 스케치 | 실습 내용 |
|---|---|
| [00_print_mac](firmware/00_print_mac/00_print_mac.ino) | 각 ESP32의 STA MAC 주소 확인 |
| [01_joystick_read](firmware/01_joystick_read/01_joystick_read.ino) | 조이스틱 중심·방향과 정지 버튼 확인 |
| [02_espnow_sender_test](firmware/02_espnow_sender_test/02_espnow_sender_test.ino) | 차량 MAC을 입력하고 시험 패킷 송신 |
| [03_espnow_receiver_test](firmware/03_espnow_receiver_test/03_espnow_receiver_test.ino) | 시험 패킷 수신·출력 |
| [controller_joystick](firmware/controller_joystick/controller_joystick.ino) | ADC를 주행 패킷으로 변환·송신 |
| [vehicle_receiver](firmware/vehicle_receiver/vehicle_receiver.ino) | 무선 명령·전진 거리 조건·수신 시간 초과를 결합 |

## 확인할 결과

정지 버튼과 수신 중단 300ms 조건에서 정지하는지 측정한다. 전방 거리 조건은 전진보다 우선하며 후진·제자리 회전은 허용한다.

[요구사항](docs/requirements.md) · [시험 절차](docs/test-plan.md) · [장비 설정](../course/CONFIGURATION.md) · [실습 기록](../course/LAB_RECORD.md)

## 다음 단계

v3에서 같은 명령 패킷에 BMI270 기울기 입력을 연결한다.

[전체 학습 흐름](../course/README.md)
