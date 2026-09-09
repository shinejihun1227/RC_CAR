# 설치와 업로드 준비

이 저장소의 핀맵은 일반 ESP32 DevKit V1 / ESP32-WROOM-32 기준이다. ESP32-C3/S3 등 다른 보드는 GPIO와 기능을 별도로 확인해야 한다.

| 설치 항목 | 사용 단계 | 안내 |
|---|---|---|
| Arduino IDE와 ESP32 보드 패키지 | 전체 | [ESP32 설치](esp32-board-package.md) |
| Pololu VL53L1X | v1, v2 차량, v4 차량 | [거리센서 라이브러리](vl53l1x-pololu.md) |
| SparkFun BMI270 Arduino Library | v3 | [IMU 라이브러리](bmi270-sparkfun.md) |
| ESP-NOW | v2, v3, v4 | [통신 설정](esp-now.md) |
| Fusion 360 | 선택 차체 설계 실습 | [설계 파일](../../hardware/README.md) |

## 업로드

1. Arduino IDE에서 보드를 ESP32 Dev Module로 선택한다.
2. USB 포트를 확인하고 모터 전원을 끈다.
3. 해당 스케치 폴더의 같은 이름 .ino 파일을 연다. v0~v4 전체를 하나의 스케치로 합치지 않는다.
4. 컴파일 후 해당 보드에 업로드한다.
5. 시리얼 출력이 있는 실습은 Arduino IDE의 시리얼 모니터를 115200 baud로 연다. f/b/l/r/s 같은 명령은 입력창에서 한 글자씩 전송한다.
6. 모터를 시험하기 전 배선표와 실제 연결을 비교한다.

무선 단계에서는 송신기와 수신기에 서로 다른 코드를 올린다. [장비별 설정](../CONFIGURATION.md)에 따라 차량 MAC 주소를 입력한다. 실습 언어와 라이브러리 호출의 구분은 [C언어 중심 안내](../ARDUINO_C_GUIDE.md)를 따른다.

공식 설치 안내: [Espressif Arduino ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html).

## 컴파일 확인 환경

수업 코드의 컴파일 확인에는 `ESP32 Dev Module`, ESP32 보드 패키지 `2.0.11`, Pololu VL53L1X `1.3.1`, SparkFun BMI270 Arduino Library `1.0.3`을 사용한다. ESP32 3.x용 수신 콜백 분기도 코드에 포함되어 있지만 전체 스케치의 보드 컴파일 확인은 2.0.11 기준이다. 실제 장비의 업로드·배선·주행 결과는 별도로 실습 기록에 남긴다.
