# ESP32 보드 패키지

## 설치

1. Arduino IDE를 연다.
2. `파일 > 환경설정`에서 추가 보드 매니저 URLs에 Espressif ESP32 보드 매니저 주소를 등록한다.
3. `도구 > 보드 > 보드 매니저`에서 `esp32 by Espressif Systems`를 설치한다.
4. 보드를 `ESP32 Dev Module`로 선택한다.
5. 시리얼 모니터 속도는 `115200 baud`로 설정한다.

## 포함 기능

- `Wire.h`: I2C 센서 통신
- `WiFi.h`: ESP32 Wi-Fi 기능
- `esp_now.h`: ESP-NOW 무선 통신

모터 전원은 업로드 중 끄고 ESP32 USB만 연결합니다.
