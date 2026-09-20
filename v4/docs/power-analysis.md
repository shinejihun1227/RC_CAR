# v4 전원 시스템 검토

## 제안 구성

```text
차량: 1.5 V AA × 4 (공칭 6 V)
  ├─ 스위치·퓨즈 ── TB6612FNG VM ── 좌우 6 V N20 모터 2개
  └─ 5 V 벅-부스트 ── 차량 ESP32 5V/VIN

조종기: 1셀 Li-ion/LiPo
  └─ 보호·충전 회로 ── 3.3 V 레귤레이터 ── 조종기 ESP32·BMI270
```

차량의 4×AA는 공칭 6 V이므로 6 V N20의 모터 전원 후보로 사용할 수 있다. TB6612FNG의 VM 허용 범위 안에 있지만, 실제 사용 가능 여부는 구매한 N20의 정지전류와 AA 셀의 순간 전류에 달려 있다. TB6612FNG의 데이터시트 정격은 평균 출력 전류 1.2 A, 피크 3.2 A이므로 이 수치를 모터 정지전류와 비교한다. [Toshiba TB6612FNG 데이터시트](https://toshiba.semicon-storage.com/info/TB6612FNG_datasheet_en_20141001.pdf?did=10660&prodName=TB6612FNG)

차량 ESP32에는 AA 팩을 직접 연결하지 않는다. 5 V 벅-부스트의 출력 전압을 USB를 연결하기 전에 측정하고, 모터 기동 때도 5 V가 유지되는지 확인한다. ESP32 모듈의 권장 동작 전압은 3.0~3.6 V이며 전원은 최소 500 mA 이상의 여유를 갖도록 설계한다. [Espressif ESP32 하드웨어 설계 지침](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32/schematic-checklist.html)

## 조종기 1셀 전원 주의

일반 Li-ion/LiPo 1셀은 공칭 약 3.6~3.7 V이고 완충 시 4.2 V가 된다. 따라서 원셀을 ESP32의 `3V3` 핀에 직접 연결하면 안 된다. 보호·충전 회로를 사용하고, 충전기와 셀 화학계열을 일치시킨 뒤, 3.3 V로 조정된 buck-boost 또는 충분한 입력 범위의 레귤레이터를 거친 출력만 ESP32와 BMI270에 공급한다. 만약 실제 셀이 LiFePO4라면 공칭 전압과 완충 전압이 다르므로 LiFePO4 전용 충전기와 해당 입력 범위의 레귤레이터를 사용한다. [Panasonic 산업용 배터리 카탈로그](https://energy.panasonic.com/dam/master/pdf/eu/catalog/old/Panasonic_Industrial-Batteries-For-Professionals_Short-Form-Catalog.pdf)

BMI270의 VDD/VDDIO 범위는 1.7~3.6 V이므로 3.3 V 레일과 맞는다. 전원 설계에서 더 큰 순간 전류 여유가 필요한 쪽은 BMI270보다 ESP32 무선 송신부다. [Bosch BMI270 제품 페이지](https://www.bosch-sensortec.com/en/products/motion-sensors/imus/bmi270)

## 결론과 확인 조건

- 차량 쪽 4×AA + TB6612FNG + 5 V 벅-부스트 구조는 조건부로 적합하다.
- 조종기 쪽은 “3.3 V 리튬이온 셀”이라고 부르지 말고, `1셀 배터리 + 보호·충전 + 3.3 V 레귤레이터 출력`으로 부품을 지정한다.
- 실제 구매 전 N20 정지전류, 엔코더 출력 전압, AA 셀 종류, 레귤레이터의 입력 범위와 출력 전류를 기록한다.
- 첫 전원 시험은 모터를 공중에 띄운 상태에서 한다. 모터 기동·정지 때 차량 5 V와 조종기 3.3 V를 측정하고, TB6612FNG와 배터리 홀더의 발열을 기록한다.
- 수동 바퀴의 접지 높이와 자유회전을 확인한다. 바퀴가 걸리거나 차체 하중을 과도하게 받으면 모터 정지전류가 증가한다.

이 문서는 부품 실측과 전기 시험 전의 설계 검토다. 시험 전에는 “전원 시스템 검증 완료”로 표현하지 않는다.
