# v2 통신 패킷

```cpp
struct ControlPacket {
  int16_t throttle;  // -255(후진) ~ +255(전진)
  int16_t steering;  // -255(좌) ~ +255(우)
  bool emergencyStop;
  uint32_t sequence;
};
```

차량은 가장 최근 패킷의 수신 시각을 저장하고, 300 ms를 넘으면 `emergencyStop`과 같은 정지 동작을 수행한다.
