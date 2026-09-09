# 차체·조종기 설계 실습

실제 부품을 측정하고 샤시·브래킷·배터리·전자부품의 장착 공간을 확인하는 자료다. 설계 코드가 실행되는 것과 실물에 맞는 것은 별도로 확인한다.

| 수업 | 파일 | 용도 |
|---|---|---|
| 기본 샤시 | [기본 프레임 생성기](fusion360/RC_CAR_Chassis_Frame_Builder.py) | 300×400mm 샤시, 레일, 범퍼, 브리지. [설명](fusion360/CHASSIS_ONLY_README.md) |
| 부품 장착 | [장착 프레임 생성기](fusion360/RC_CAR_Mounting_Frame_Builder.py) | N20 브래킷, 배터리 트레이, 전장 데크, ToF 장착부 |
| 로버형 확장 | [로버 샤시 생성기](fusion360/RC_CAR_Rover_Chassis_Builder.py) | 320×420mm 차체·상부 데크·작업 장치 인터페이스. [설명](fusion360/ROVER_CHASSIS_README.md) |
| 부품 배치 | [참조 부품](reference-parts/README.md) | 모터·바퀴·보드·홀더 외곽 STL와 배치 생성기 |
| 조종기·모터 고정 | [STL 안내](stl/README.md) | 케이스 상·하판과 N20 마운트 |

## 실행 순서

1. 실제 모터·축·장착홀, 보드 USB 위치, 배터리 홀더를 캘리퍼스로 측정한다.
2. Fusion 360의 Scripts and Add-Ins에서 필요한 Python 생성기를 등록·실행한다.
3. 새 Design에서 컴포넌트와 파라미터를 확인한다.
4. 브래킷 또는 홀 시편부터 시험 출력한다.
5. 간섭과 체결을 확인한 뒤 전체 차체에 적용한다.

현재 대형 프레임은 220×220mm급 프린터보다 크다. 프린터에 맞는 크기 변경·분할 등 제작 방식을 먼저 정한다. v0~v3의 소형 TT 실습에 대형 로버 프레임이 필수인 것은 아니다.

장착 프레임은 N20 브래킷을 기본 표시하고 TT 브래킷과 참조 외곽을 숨기는 구성이다. v4에서 모터를 교체할 때 엔코더 PCB·배선 공간을 확인한다.

[치수 출처와 확인 항목](fusion360/DIMENSION_SOURCES.md)
