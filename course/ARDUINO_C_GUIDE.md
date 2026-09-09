# Arduino에서 배우는 C언어 중심 RC카 실습

수업은 **ESP32 + Arduino IDE + 시리얼 모니터**로 진행한다. 기존 ESP32 DevKit의 배선과 부품을 사용한다. Arduino IDE를 쓴다고 보드를 Arduino Uno로 바꾸는 것은 아니다.

## 언어와 라이브러리의 구분

학생이 작성하는 제어 로직은 C 문법으로 설명한다. `int`, `float`, `const`, `if`, `switch`, `for`, 함수, 배열, `typedef struct`, 포인터를 순서대로 배운다. 직접 클래스를 만들거나 상속·템플릿을 배우는 과정은 포함하지 않는다.

Arduino의 `.ino` 파일은 전처리 후 C++로 컴파일된다. 이 저장소는 **C 문법 중심의 Arduino 스케치**이며 순수 ISO C 프로그램은 아니다. `Serial.begin()`, `Wire.begin()`, `tof.read()`, `bmi.getSensorData()`는 Arduino와 센서 라이브러리가 제공하는 C++ API다. 이 호출부는 장치와 데이터를 주고받는 방법으로 설명하고, 학생은 반환값을 이용해 판단·계산·출력을 작성한다. [Arduino 공식 빌드 과정](https://docs.arduino.cc/arduino-cli/sketch-build-process/)

`.ino` 확장자를 `.c`로 바꾸지 않는다. Arduino API를 사용하는 현재 스케치를 그대로 Arduino IDE에서 열고 **ESP32 Dev Module**을 선택한다. Fusion 360 폴더의 Python은 설계 도구이며 차량 코딩 실습과 별개다.

## 단계별 C 문법과 수정 과제

| 단계 | 먼저 설명할 문법 | 코드에서 읽을 부분 | 학생이 직접 바꿀 내용 | 확인할 결과 |
|---|---|---|---|---|
| [v0](../v0/README.md) | 자료형, 상수, 함수·인자, if/else, switch, break, return | `setMotor()`, `drive()`, 명령 분기 | `DRIVE_SPEED`를 120과 180으로 바꾸고 방향별 인자를 설명 | PWM과 실제 속도 차이, f/b/l/r/s 동작 |
| [v1](../v1/README.md) | bool, 비교·논리 연산, while, 상태 변수, unsigned long | `currentCommand`, `updateDistance()`, `obstacleTooClose()` | `STOP_MM`를 300으로 바꾸고 경계값 조건을 설명 | 한 번 f를 보내도 거리 변화로 정지하는지, 새 f 전까지 정지 유지 |
| [v2](../v2/README.md) | 배열, typedef struct, 고정 폭 정수, 주소 &, sizeof, memcpy, 콜백 함수 | `ControlPacket`, `axisToCommand()`, `onReceive()` | 조이스틱 데드존을 120에서 200으로 변경 | 중립 흔들림, 버튼 정지, 통신 끊김 후 정지 |
| [v3](../v3/README.md) | float 계산, fabsf/atan2/sqrt, for 누적·평균, 형 변환 | `angleToCommand()`, `calibrateNeutral()` | 데드존 8도와 12도를 비교하고 100회 평균 계산 설명 | 기울기→명령의 변화, 부팅 시 중립 보정 |
| [v4](../v4/README.md) | 배열 인덱스, volatile, ISR, 임계 구역, 시간 차, 누적값 | `pulses[]`, `updatePid()`, P/I/D 계산 | 바퀴 1회전 펄스를 실측해 PPR 입력, Kp부터 조정 | 네 RPM 기록, 목표값과 오차·진동 비교 |

`map()`, `constrain()`, `millis()`는 Arduino 함수·매크로다. `fabsf()`, `atan2()`, `sqrt()`는 C 수학 함수다. `bool`은 C99의 `<stdbool.h>`, `int16_t`·`uint32_t`는 `<stdint.h>`와 연결해 설명한다. `volatile`만으로 동시 접근이 안전해지지 않으므로 v4에서는 인터럽트와 공유하는 값을 임계 구역에서 복사하는 이유도 다룬다.

## 첫 코드 읽기: 변수 → 함수 → 명령 분기

다음은 [v0 기본 주행](../v0/firmware/02_basic_drive/02_basic_drive.ino)의 일부다. 전체 실행에는 해당 스케치의 핀 설정과 함수가 필요하다.

```c
const int DRIVE_SPEED = 180;

void drive(int leftSpeed, int rightSpeed) {
  setMotor(AIN1, AIN2, PWMA, leftSpeed);
  setMotor(BIN1, BIN2, PWMB, rightSpeed);
}
```

`const int`는 바꾸지 않을 정수 값을 선언한다. `void`는 반환값이 없다는 뜻이고 두 인자는 왼쪽·오른쪽 모터 명령이다. 양수는 전진, 음수는 후진, 0은 정지로 처리한다. `loop()`의 `switch`에서 문자 명령마다 이 함수를 다른 인자로 호출한다.

초반에는 `pinMode()`를 핀마다 작성해 입출력을 익힌다. v2에서는 MAC 주소와 패킷으로 배열·구조체를 배우고, v4에서 같은 처리를 네 바퀴에 반복하며 배열 인덱스를 활용한다.

## 무선 코드 읽기: 구조체 → 주소 → 전송

아래는 송신기의 일부다. `ControlPacket` 선언과 ESP-NOW 초기화는 [v2 조이스틱 조종기](../v2/firmware/controller_joystick/controller_joystick.ino)에 있다.

```c
ControlPacket packet = {0};
packet.throttle = axisToCommand(analogRead(JOY_Y));
packet.steering = axisToCommand(analogRead(JOY_X));
packet.emergencyStop = !digitalRead(STOP_BUTTON);
packet.sequence = sequenceNumber++;

esp_now_send(vehicleMac, (const uint8_t *)&packet, sizeof(packet));
```

`packet.throttle`은 구조체 필드 접근이고, `tof.read()`는 센서 객체의 기능 호출이다. 같은 점 표기가 나오지만 역할이 다르다. `&packet`은 메모리 주소, `(const uint8_t *)`는 그 주소를 읽기 전용 바이트 포인터로 전달하는 형 변환, `sizeof(packet)`은 패딩을 포함한 구조체 크기다. 송신기·수신기의 필드 순서와 자료형을 함께 유지한다. 이 원시 구조체 전송은 같은 ESP32 환경을 전제로 하며 다른 기종 간의 범용 데이터 형식으로 설명하지 않는다.

## 실행과 결과 확인

1. 각 버전 README의 표에서 단품 스케치를 먼저 연다. `.ino` 파일과 폴더 이름을 같게 유지한다.
2. [설치 안내](setup/README.md)에 따라 컴파일하고 해당 ESP32에 업로드한다.
3. 시리얼 모니터를 115200 baud로 연다. 명령 입력은 한 글자씩 전송한다.
4. 한 번에 설정 하나를 바꾸고 예상 결과와 실제 측정값을 [실습 기록](LAB_RECORD.md)에 적는다.
5. 통합 스케치로 이동해 기존 동작과 정지 조건을 다시 확인한다.

v0 단품 모터 테스트는 자동으로 전후진한다. 모든 모터 실습의 첫 시험은 바퀴를 띄운 상태에서 진행한다. v1·v2·v4 통합 차량은 거리 값이 없거나 오래된 경우에도 전진을 차단한다. v4의 `PULSES_PER_WHEEL_REV = 1.0f`는 실측 전 자리값이므로 보정 없이 RPM 정확도를 평가하지 않는다.
