# RC_CAR Python 실습

이 폴더의 Python 파일은 ESP32에 연결된 부품을 PC에서 직접 제어하는 코드가 아니다.
현재 프로젝트의 센서와 엔코더는 ESP32의 I²C·GPIO에 연결되어 있고, 센서 읽기는 Arduino 펌웨어가 담당한다.
Python은 ESP32가 USB 시리얼로 출력하는 값을 받아 계산·판정·기록하는 실습에 사용한다.

## 설치

```bash
python -m pip install -r python/requirements.txt
```

## 실습 순서

먼저 Arduino IDE로 해당 펌웨어를 ESP32에 업로드하고, Arduino IDE의 시리얼 모니터를 닫은 뒤 Python을 실행한다.

### 1. BMI270 실습

업로드할 펌웨어:

```text
v3/firmware/00_bmi270_read/00_bmi270_read.ino
```

실행:

```bash
python python/01_bmi270_serial_lab.py --port COM5
```

Python이 가속도·자이로 로그를 읽고 Roll·Pitch·STOP/FORWARD/BACKWARD/LEFT/RIGHT를 계산한다.

### 2. VL53L1X 거리 실습

업로드할 펌웨어:

```text
v1/firmware/01_distance_read/01_distance_read.ino
```

실행:

```bash
python python/02_vl53l1x_serial_lab.py --port COM5
```

`500mm` 이내는 WARNING, `200mm` 이내는 STOP으로 표시한다. 이 값은 현재 안전 정지 코드의 기준과 맞춘다.

### 3. 엔코더 RPM 실습

업로드할 펌웨어:

```text
v4/firmware/00_encoder_read/00_encoder_read.ino
```

실행:

```bash
python python/03_encoder_rpm_serial_lab.py --port COM5
```

네 바퀴의 RPM과 가장 빠른 바퀴·가장 느린 바퀴의 편차를 확인한다.

### 4. TB6612FNG 안전 주행 실습

업로드할 펌웨어:

```text
v1/firmware/02_safety_stop/02_safety_stop.ino
```

실행:

```bash
python python/04_motor_safety_serial_lab.py --port COM5
```

키를 입력한다.

```text
f: 전진
b: 후진
l: 좌회전
r: 우회전
s: 정지
q: 종료 후 정지 명령 전송
```

이 파일은 ESP-NOW를 대신하지 않는다. ESP-NOW 최종 통합은 기존 Arduino 파일인 `v3/firmware/controller_bmi270`과 `v4/firmware/vehicle_encoder_receiver`가 담당한다.
