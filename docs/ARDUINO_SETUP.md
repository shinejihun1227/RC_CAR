# Arduino IDE 실습 환경

## 공통 설정

1. Arduino IDE에서 Espressif ESP32 보드 패키지를 설치한다.
2. 보드로 `ESP32 Dev Module`을 선택한다.
3. 업로드 전에는 모터 전원을 끄고 ESP32 USB만 연결한다.
4. 시리얼 모니터는 115200 baud로 연다.

## 라이브러리 안내

라이브러리 설치와 버전별 사용 위치는 루트의 [library](../library/README.md) 폴더에서 관리합니다.

- [ESP32 보드 패키지](../library/esp32-board-package.md)
- [VL53L1X by Pololu](../library/vl53l1x-pololu.md)
- [ESP-NOW](../library/esp-now.md)
- [SparkFun BMI270 Arduino Library](../library/bmi270-sparkfun.md)

## 업로드 순서

1. v0의 모터 테스트를 통과한다.
2. v1에서 VL53L1X 단독 측정을 통과한다.
3. v2 조종기와 차량의 MAC 주소를 교환한 뒤 통신을 확인한다.
4. v3에서 BMI270 중립 자세와 제스처 임계값을 보정한다.
