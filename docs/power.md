# 전원 연결

## 12 V 어댑터로 실습할 때

Mega와 Uno는 각각 USB로 켭니다. 12 V 5 A 어댑터는 차량의 L298N `12V`와 `GND`에만 연결합니다. Mega `GND`를 L298N `GND`에도 연결해야 방향·속도 신호가 전달됩니다.

L298N의 `5V-EN` 캡을 끼우면 보드가 12 V 입력으로 제어 회로용 5 V를 만듭니다. 그래서 **Mega `5V`와 L298N `5V`를 연결하지 않습니다.** `ENA`·`ENB` 캡은 빼고 Mega D5·D6에 연결합니다. 이 점퍼 구성은 L298N 입력이 12 V 이하일 때 사용합니다. [L298N 모듈 설명](https://docs.sunfounder.com/projects/sf-components/en/latest/component_l298n_module.html)

무선 모듈 본체에는 3.3 V가 필요합니다. 5 V 입력형 소켓 어댑터가 있다면 Mega 또는 Uno의 5 V에서 어댑터로 전원을 보내고, 무선 본체는 그 어댑터에 꽂습니다. 거리 센서 기판은 **5 V 입력형인지 확인한 뒤** Mega 5 V에 연결합니다. [nRF24L01 전원 범위](https://devzone.nordicsemi.com/cfs-file/__key/support-attachments/beef5d1b77644c448dabff31668f3a47-aad1a46f307945a7b0204fd969e86bdf/content.pdf)

`12 V 5 A`의 5 A는 어댑터가 낼 수 있는 최대 전류입니다. 모터에 5 A가 항상 흐른다는 뜻은 아닙니다. 다만 모터의 정격 전압과 스톨 전류를 확인한 뒤 L298N과 함께 쓸 수 있는지 판단해야 합니다.

## 차량에 3S 배터리를 실을 때

3S 리튬이온 팩은 완충 시 12.6 V까지 올라갑니다. 이때는 L298N의 `5V-EN` 캡을 **빼고**, 별도의 5 V 강압 모듈로 L298N `5V` 단자에 제어 회로 전원을 넣습니다. 배터리 전원은 다음처럼 나눕니다.

```text
3S 배터리 → 스위치·퓨즈 → L298N 12V → 모터
          └→ 5 V 강압 모듈 → Mega의 USB 전원 경로
                            ├→ L298N 5V (5V-EN 캡 제거)
                            └→ 무선용 3.3 V 어댑터
```

배터리 음극, 강압 모듈 GND, Mega GND, L298N GND는 공통으로 연결합니다. Mega의 `5V` 핀에 외부 전원을 바로 넣는 배선은 피하고, 조정된 5 V를 적절한 USB 전원 연결로 공급하는 방식을 권합니다. PC USB와 배터리 쪽 5 V 전원을 함께 꽂기 전에는 전원 선택·역류 방지 방법을 정해야 합니다. [Arduino 전원 핀 안내](https://docs.arduino.cc/learn/electronics/power-pins/)

배터리팩은 3S 보호회로와 맞는 방전 정격을 확인하고, 충전에는 해당 팩용 **12.6 V 리튬이온 CC/CV 충전기**를 사용합니다. 가지고 있는 12 V 5 A 어댑터는 충전기가 아닙니다. 강압 모듈의 용량과 배터리 방전 정격은 모터 사양을 확인한 뒤 정할 수 있습니다.
