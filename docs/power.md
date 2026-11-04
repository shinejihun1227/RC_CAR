# 전원 연결

LM2596는 승강압(벅부스트)이 아니라 **강압(벅)** 모듈입니다. 가진 12 V 5 A 어댑터에서 5 V를 만들어 빨간 L298N의 제어 회로에 넣을 수 있습니다. LM2596 기판의 단자가 `IN+`, `IN−`, `OUT+`, `OUT−`로 표시된 제품을 기준으로 설명합니다. [TI LM2596](https://www.ti.com/product/LM2596)

## 지금: 12 V 어댑터, LM2596, 두 Arduino의 USB

1. 모든 전원을 끄고 **L298N `5V-EN` 캡을 뺍니다.** `ENA`와 `ENB` 캡도 빼서 Mega의 속도 제어선을 꽂습니다. 세 캡은 서로 다른 곳입니다.
2. 어댑터 출력 `+`를 L298N `12V`와 LM2596 `IN+`로 나눕니다. 어댑터 출력 `−`를 L298N `GND`와 LM2596 `IN−`로 나눕니다.
3. 다른 부품을 LM2596 출력에 연결하지 않은 채 어댑터를 켜고, 멀티미터로 `OUT+`와 `OUT−` 사이를 측정하면서 가변저항을 돌려 **5.0 V**로 맞춥니다. `IN`/`OUT`이나 극성을 거꾸로 연결하지 않습니다.
4. 다시 전원을 끈 뒤 LM2596 `OUT+` → L298N `5V`, `OUT−` → L298N `GND`를 연결합니다.
5. 차량 Mega와 리모컨 Uno는 각각 PC의 **USB로 전원 공급**합니다. Mega `GND` → L298N `GND`는 연결합니다. **Mega `5V`와 L298N `5V`는 연결하지 않습니다.** Uno와 차량 사이에는 전원선·GND선을 잇지 않습니다.

```text
12 V 어댑터 (+) ─┬─ L298N 12V ─ OUT1~4 ─ 모터 2개
                └─ LM2596 IN+
12 V 어댑터 (−) ─┬─ L298N GND ─ Mega GND
                └─ LM2596 IN−
LM2596 OUT+ (측정한 5.0 V) ─ L298N 5V    [5V-EN 캡 제거]
LM2596 OUT− ──────────────── L298N GND
Mega USB ─ PC/USB 전원       Uno USB ─ 별도 PC/USB 전원
```

LM2596에는 **모터 전류를 흘리지 않습니다.** 모터는 L298N `12V` 입력의 전원을 받습니다. 어댑터의 `5 A`는 공급 가능한 최대치이지 모터에 강제로 5 A가 흐른다는 뜻은 아닙니다. 두 모터의 정격 전압이 12 V인지, 각 모터의 **스톨 전류**가 L298N 채널 허용치를 넘지 않는지 확인해야 합니다. [L298N 모듈 설명](https://docs.sunfounder.com/projects/sf-components/en/latest/component_l298n_module.html)

무선 본체에는 **안정적인 3.3 V**가 필요합니다. 5 V 입력형 nRF24L01 소켓 어댑터가 있다면 각 Arduino의 `5V·GND` → 해당 소켓 어댑터 `VCC·GND`로 공급하고 무선 본체를 어댑터에 꽂습니다. **LM2596의 5 V를 무선 본체 `VCC`에 직접 넣지 않습니다.** PA/LNA형 본체를 Arduino의 전류가 작은 3.3 V 핀에서 직접 구동하지 않는 구성을 권합니다. [nRF24L01+ PA/LNA 모듈 사양](https://www.sirius-pcb.com/download/YTozOntzOjY6Im9iamVjdCI7czo4OiJwcm9kdWN0cyI7czo5OiJmaWxlX3R5cGUiO3M6OToiZG9jdW1lbnRzIjtzOjc6ImZpbGVfaWQiO3M6NToiMjIxNjYiO30%3D), [Mega 핀 전원 한도](https://docs.arduino.cc/resources/pinouts/A000067-full-pinout.pdf)

## 나중: 차량에 3S 리튬이온 팩을 실을 때

3S 배터리는 완충 시 약 **12.6 V**입니다. 이때도 L298N `5V-EN`을 빼고, 배터리 `+`를 퓨즈·전원 스위치 뒤에서 L298N `12V`와 LM2596 `IN+`로 나눕니다. 배터리 `−`는 L298N `GND`와 LM2596 `IN−`로 연결합니다. LM2596 출력은 5.0 V로 맞춰 L298N `5V`와 **Mega 전용 USB 전원 케이블**에 공급합니다. Mega `GND`와 L298N `GND`는 공통입니다. 조종기 Uno는 차량 배터리와 연결하지 않고 별도 USB 전원을 사용합니다.

PC USB와 배터리에서 만든 5 V USB 전원을 Mega에 동시에 넣지 않습니다. 업로드할 때는 차량 배터리를 끄고 PC USB를 사용합니다. 배터리 주행 때는 PC 케이블을 빼고 5 V USB 전원 케이블로 교체합니다. Mega `5V` 핀에 외부 5 V를 직접 넣는 방식은 피합니다. [Arduino 전원 핀 안내](https://docs.arduino.cc/learn/electronics/power-pins/)

3S 팩은 보호회로(BMS)와 모터에 맞는 방전 정격을 확인하고 **12.6 V 리튬이온 CC/CV 충전기**로 충전합니다. 지금의 12 V 5 A 어댑터는 배터리 충전기가 아닙니다. 리모컨의 KCD1은 아래 코드에서 **조종 모드 신호 입력**으로 사용하며, 이 12 V·5 V 전원선에는 넣지 않습니다.
