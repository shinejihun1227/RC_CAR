# 실습 진행

스케치 파일은 같은 이름의 폴더째 Arduino IDE에서 엽니다. 차량 코드는 **Arduino Mega 2560**, 조종기 코드는 **Arduino Uno**를 선택하고, 시리얼 모니터 속도는 **115200 bps**로 맞춥니다. 배선은 [배선표](wiring.md)를 참고하세요.

## 1. 모터 방향 맞추기

모터 라벨이나 판매 사양에서 정격 전압을 먼저 확인합니다. 12 V 모터라면 바퀴를 바닥에서 띄우고 [motor_practice.ino](../firmware/motor_practice/motor_practice.ino)를 Mega에 올립니다. Mega는 USB로, L298N은 12 V 어댑터로 켭니다.

시리얼 모니터에서 `1`은 모터 A, `2`는 모터 B, `3`은 두 모터 전진, `4`는 두 모터 후진입니다. 한 번 보낼 때마다 0.6초만 돌고 멈춥니다. `x`를 보내도 정지합니다. 전진 명령에서 두 바퀴가 차량을 같은 방향으로 보내는지 살펴보고, 반대로 도는 쪽은 모터 선 한 쌍을 바꾸거나 차량 코드의 `LEFT_MOTOR_INVERTED` 또는 `RIGHT_MOTOR_INVERTED`를 바꿉니다.

## 2. 거리 센서 읽기

L298N의 12 V 전원은 빼고 거리 센서와 Mega만 연결합니다. 센서 기판의 전원 입력 범위를 확인한 뒤 [distance_practice.ino](../firmware/distance_practice/distance_practice.ino)를 올립니다. 물체를 센서 앞에서 움직이면 시리얼 모니터의 `Distance: ... mm` 값도 바뀌어야 합니다.

`VL53L0X not found`가 나오면 전원, GND, SDA(D20), SCL(D21)을 다시 확인합니다.

## 3. 무선으로 명령 보내기

차량과 조종기에 무선 모듈을 하나씩 연결합니다. PA/LNA형 모듈에는 각각 안정적인 3.3 V 전원이 필요합니다. 조종기 Uno에는 [controller_transmitter.ino](../firmware/controller_transmitter/controller_transmitter.ino), 차량 Mega에는 [vehicle_receiver.ino](../firmware/vehicle_receiver/vehicle_receiver.ino)를 올립니다. 이때는 아직 **모터 12 V 전원을 연결하지 않습니다.**

두 코드의 `VEHICLE_ID`와 `RADIO_CHANNEL` 값이 같은지 확인합니다. Uno 시리얼 모니터에 `w`를 보내면 Uno에는 `ack=YES`, Mega에는 `link=OK`와 `cmd=60,0`이 보여야 합니다. 연결이 안 되면 두 모듈의 전원과 CE·CSN·SPI 배선부터 확인합니다. RF24 라이브러리에 포함된 [GettingStarted 예제](https://github.com/nRF24/RF24/blob/master/examples/GettingStarted/GettingStarted.ino)로 무선 모듈 두 개만 따로 확인해도 됩니다.

## 4. 바퀴를 띄우고 함께 동작시키기

거리값과 무선 명령이 모두 들어오면 모터 전원을 연결합니다. Uno에 `w`, `s`, `a`, `d`를 하나씩 보내고 바퀴 움직임을 봅니다. 시리얼 명령은 0.8초 후 자동으로 풀립니다. 조종기 전원을 빼거나 무선 연결을 끊으면 차량도 마지막 명령을 받은 뒤 약 0.3초가 지나 정지해야 합니다.

센서 앞 30 cm 이내에 물체를 놓으면 전진 바퀴가 멈춥니다. 이 상태에서 후진은 가능합니다. 물체를 40 cm 밖으로 옮기면 다시 전진할 수 있습니다. 바닥에서 주행한 뒤에는 실제 정지 거리에 맞춰 차량 코드의 `STOP_DISTANCE_MM`, `RESUME_DISTANCE_MM`, `MAX_PWM`을 조정합니다.

## 조이스틱을 연결한 뒤

Uno의 `A0`에 VRx, `A1`에 VRy를 연결하고 조종기 코드의 `USE_JOYSTICK`을 `1`로 바꿉니다. 모터 전원을 빼고 조이스틱을 놓았을 때 `cmd=0,0`에 가까운지 먼저 봅니다. 조이스틱 방향이 반대면 송신 코드의 축 반전 설정을 조정합니다.

## 막히는 곳

| 보이는 현상 | 확인할 곳 |
|---|---|
| Mega는 켜졌는데 모터가 돌지 않음 | L298N 12 V, `5V-EN` 캡, Mega와 L298N의 공통 GND, `ENA/ENB` 배선 |
| 모터가 최고속으로만 돎 | `ENA/ENB` 점퍼캡을 뺐는지 |
| 거리값이 안 나옴 | 센서 기판 전원, SDA D20, SCL D21 |
| `ack=NO` | 무선 모듈 두 개의 3.3 V 전원, SPI 배선, 양쪽 코드의 ID·채널 |
| `ack=YES`인데 전진하지 않음 | Mega의 `blocked` 값, 센서 앞 물체, 모터 전원 |

스케치 컴파일은 확인했지만, 위의 실제 동작 결과는 차량을 조립하면서 기록해야 합니다.
