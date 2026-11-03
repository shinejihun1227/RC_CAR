# 실습 순서와 업로드할 코드

Arduino IDE에서 `.ino` 파일을 열고 보드를 선택합니다. 차량은 **Arduino Mega 2560**, 조종기는 **Arduino Uno**입니다. 코드를 올릴 때는 해당 보드의 USB 케이블을 PC에 꽂고, 시리얼 모니터를 **115200 bps**로 엽니다. 라이브러리 관리자에서 **RF24**와 **VL53L0X (Pololu)**를 설치하세요. [배선표](wiring.md)와 [전원 연결](power.md)을 먼저 확인합니다.

| 순서 | Mega에 올릴 코드 | Uno에 올릴 코드 | 필요한 연결 |
|---|---|---|---|
| 1. 모터 | [motor_practice.ino](../firmware/motor_practice/motor_practice.ino) | 없음 | Mega, L298N, 모터 두 개 |
| 2. 거리 | [distance_practice.ino](../firmware/distance_practice/distance_practice.ino) | 없음 | Mega, VL53L0X |
| 3. 무선 | [radio_receive_practice.ino](../firmware/radio_receive_practice/radio_receive_practice.ino) | [radio_send_practice.ino](../firmware/radio_send_practice/radio_send_practice.ino) | Mega·Uno, 무선 본체 두 개와 각 전원 어댑터 |
| 4. 조이스틱(선택) | 없음 | [joystick_practice.ino](../firmware/joystick_practice/joystick_practice.ino) | Uno, 5 V 호환 조이스틱 1~2개 |
| 5. 전체 주행 | [vehicle_receiver.ino](../firmware/vehicle_receiver/vehicle_receiver.ino) | [controller_transmitter.ino](../firmware/controller_transmitter/controller_transmitter.ino) | 앞의 모든 부품; 조이스틱은 선택 |

앞 실습의 코드를 지우는 별도 작업은 없습니다. 새 스케치를 업로드하면 그 보드에 있던 코드가 바뀝니다. **3번 무선 실습용 코드 두 개는 한 쌍**, **5번 전체 주행용 코드 두 개는 한 쌍**입니다. 3번 송신기와 5번 수신기를 섞으면 패킷 형식이 달라 통신 실습이 되지 않습니다.

## 1. 모터 방향

모터 라벨의 정격 전압과 스톨 전류를 확인한 다음, 12 V 정격이면 [12 V 어댑터 배선](power.md#지금-12-v-5-a-어댑터와-usb)을 사용합니다. 바퀴를 바닥에서 띄우고 Mega에 `motor_practice.ino`를 올립니다. L298N `5V-EN`은 끼우고 `ENA`·`ENB`는 뺍니다.

Mega 시리얼 모니터에 `1`을 보내면 A, `2`는 B, `3`은 양쪽 전진, `4`는 양쪽 후진입니다. 각 명령은 **0.6초**만 동작한 뒤 멈추며 `x`는 정지입니다. 전진 방향이 반대인 모터는 그 모터의 선 두 가닥을 바꿔 꽂습니다. 이 단계에서 바꿨다면 전체 주행 코드의 반전 설정은 기본값으로 둡니다.

## 2. 거리 센서

L298N 모터 전원을 끄고 센서만 Mega에 연결합니다. 센서 보드가 5 V 입력형인지 확인하고 `distance_practice.ino`를 Mega에 올립니다. 앞에 물체를 움직일 때 `Distance: ... mm`가 변해야 합니다. `VL53L0X not found`라면 전원, GND, SDA D20, SCL D21을 확인합니다.

## 3. 무선 모듈 두 개

두 Arduino에 무선 본체와 각각의 **안정적인 3.3 V 전원 어댑터**를 연결합니다. L298N 모터 전원은 꺼 둡니다. Mega에 `radio_receive_practice.ino`, Uno에 `radio_send_practice.ino`를 올립니다. 양쪽 시리얼 모니터를 열면 Uno에는 0.5초마다 `Sent #... ack=YES`, Mega에는 `Received #...`가 나와야 합니다. `ack=NO`라면 5 V 입력형 어댑터의 입력, 본체의 3.3 V, CE·CSN, SPI 핀을 확인합니다. 무선 본체가 하나뿐이면 이 실습에는 두 번째 본체가 필요합니다.

## 4. 조이스틱 입력(조이스틱이 있을 때만)

1번 조이스틱은 Uno `A0`에 `VRx`, `A1`에 `VRy`, 선택적으로 `D2`에 `SW`를 연결합니다. 2번은 `A2`에 `VRx`, `A3`에 `VRy`, 선택적으로 `D3`에 `SW`를 연결합니다. 두 보드가 5 V 입력형이면 각 보드의 `5V`와 `GND`도 Uno 전원에 연결합니다. `joystick_practice.ino`를 Uno에 올리면 두 조이스틱의 X/Y 값과 버튼 상태를 한 줄에 표시합니다. 막대를 놓았을 때 각 축이 대략 **512**이고 움직이면 0~1023 사이에서 변하는지 확인하세요. 스틱을 누르면 `PRESSED`가 나옵니다. 중앙값은 제품마다 조금 다릅니다. 이 실습에는 무선 모듈이나 Mega가 필요 없습니다.

## 5. 전체 주행

Mega에 `vehicle_receiver.ino`, Uno에 `controller_transmitter.ino`를 올립니다. 먼저 모터 전원을 끄고, Uno 시리얼 모니터에서 `w`를 보내 Uno의 `ack=YES`, Mega의 `link=OK`와 `cmd=60,0`이 나오는지 확인합니다. 양쪽 코드의 `VEHICLE_ID`와 `RADIO_CHANNEL`은 같아야 합니다.

그 다음 바퀴를 띄운 채 모터 전원을 켭니다. Uno에 `w`(전진), `s`(후진), `a`·`d`(좌우 회전), `x`(정지)를 보냅니다. 명령은 **0.8초** 뒤 자동으로 풀립니다. 무선 연결이 끊어지면 차량은 마지막 명령 후 약 **0.3초** 안에 정지하도록 작성되어 있습니다. 장애물을 30 cm 안에 놓으면 전진이 막히고 후진은 가능합니다. 40 cm 밖으로 치우면 전진이 다시 허용됩니다. 실제로 멈추는 거리를 보고 `STOP_DISTANCE_MM`, `RESUME_DISTANCE_MM`, `MAX_PWM`을 조정하세요.

차량을 여러 대 만들면 각 Mega 코드의 `VEHICLE_ID`를 **1~16처럼 서로 다르게** 바꿔 올립니다. 조종하려는 차량의 ID로 Uno 코드의 `VEHICLE_ID`도 바꿔 올립니다. 같은 ID의 차량을 두 대 켜면 둘 다 같은 명령을 받을 수 있습니다. 한 조종기로 번갈아 제어하는 경우에도 차량 ID를 구분하세요.

조이스틱으로 주행한다면 Uno 코드의 `USE_JOYSTICK`을 `1`로 바꿔 다시 올립니다. 기본값 `0`은 시리얼 입력용입니다. 현재 차량 주행 코드는 **1번 조이스틱 A0/A1만 사용**합니다. 2번 조이스틱과 `SW` 버튼은 연결해도 주행이나 로봇팔을 움직이지 않습니다. 처음에는 모터 전원을 끄고 1번 스틱 중앙에서 `cmd=0,0`에 가까운지 확인합니다.

## 3S 배터리로 옮길 때

코드는 5번과 같습니다. 바뀌는 것은 **전원 배선**입니다. 배터리 완충 시 12.6 V이므로 L298N `5V-EN`을 빼고, LM2596 `OUT+`를 5.0 V로 맞춘 뒤 L298N `5V`와 Mega USB 전원에 공급합니다. 모터에는 배터리 전원을 L298N `12V` 단자로 직접 보냅니다. 자세한 단자별 연결은 [전원 연결](power.md#나중-차량에-3s-리튬이온-팩을-실을-때)을 따릅니다.

## 막히는 곳

| 보이는 현상 | 확인할 곳 |
|---|---|
| Mega는 켜졌는데 모터가 돌지 않음 | 모터 정격 전압, L298N 모터 전원, `5V-EN` 상태, Mega와 L298N 공통 GND, `ENA/ENB` 배선 |
| 모터가 최고속으로만 돎 | `ENA/ENB` 점퍼캡을 뺐는지 |
| 거리값이 안 나옴 | 센서 보드 입력 전압, SDA D20, SCL D21 |
| 무선에서 `ack=NO` | 무선 본체 두 개, 각 3.3 V 전원, CE·CSN·SPI 배선 |
| `ack=YES`인데 전진하지 않음 | Mega의 `blocked` 값, 센서 앞 물체, 모터 전원 |

스케치 컴파일은 확인했지만, 실제 모터 방향과 전원 동작은 실물 배선 후 확인해야 합니다.
