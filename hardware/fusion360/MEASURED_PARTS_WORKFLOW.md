# 실측 부품을 기준으로 Fusion 360 차체 설계하기

대상은 **v4 N20 엔코더 2구동륜 차량의 모터 장착·부품 배치 실습**이다. 뒤쪽에는 별도 모터가 없는 수동 바퀴 2개를 사용한다. v0~v3 TT 2륜 차량과 v2·v3 조종기에서 달라지는 부품도 아래에 구분했다.

구매 부품을 실제 크기의 참조 컴포넌트로 배치하고, 그 주변에 모터 마운트·배터리 트레이·전자부품 데크·차체를 설계한다. **부품의 실측치, 끼워 맞춤 여유, 차체 설계값은 서로 다른 값으로 관리한다.**

현재 구매 목록에는 정확한 제품 URL이 기록되지 않았다. 기존 참조 STL과 생성기의 기본 치수는 구매품의 실측값이 아니다. 이 안내는 실제 Fusion 조립이나 시험 출력이 완료되었다는 뜻이 아니다.

## 1. Fusion에 배치할 부품과 현재 파일

수량은 v4 차량 1대 기준이다. 동일 부품은 모델 하나를 확인한 뒤 필요한 수만큼 배치하되, 실제 구매품의 편차도 확인한다.

| 부품 | 수량 | 현재 사용할 수 있는 파일 | 실측으로 확인할 핵심 |
|---|---:|---|---|
| N20 엔코더 기어드모터 | 2 | [N20_encoder_motor_reference.stl](../reference-parts/N20_encoder_motor_reference.stl) | 좌우 기어박스·본체·엔코더의 외곽, 출력축 중심·지름·D컷·돌출 길이, 케이블 출구 |
| N20 호환 구동 바퀴 | 2 | [wheel_70mm_reference.stl](../reference-parts/wheel_70mm_reference.stl) | 좌우 타이어 지름·폭, 허브 돌출, 축 구멍 형상·깊이, 모터에 끼운 뒤 안쪽 면 위치 |
| 수동 바퀴 | 2 | 구매품 실측 후 별도 참조 | 뒤쪽 접지 높이·축/캐스터 간섭·자유회전 상태 |
| N20 브래킷 | 2 | [N20_motor_bracket_reference.stl](../reference-parts/N20_motor_bracket_reference.stl) | 바닥 체결홀, 축 중심 높이, 엔코더 간섭. 금속 구매품과 출력 마운트는 구분 |
| ESP32 DevKit V1 | 1 | [ESP32_DevKit_V1_reference.stl](../reference-parts/ESP32_DevKit_V1_reference.stl) | 보드 외곽, 핀헤더 포함 위·아래 높이, 실제 장착홀, USB 포트·플러그 공간 |
| TB6612FNG 모듈 | 1 | [TB6612FNG_breakout_reference.stl](../reference-parts/TB6612FNG_breakout_reference.stl) | 모듈 PCB 외곽·홀·핀헤더·연결 케이블 높이 |
| AA 4개 홀더 | 1 | [AA_4cell_holder_reference.stl](../reference-parts/AA_4cell_holder_reference.stl) | 배터리·뚜껑·스위치 포함 외곽, 체결부, 배터리 교체 방향 |
| 5V 벅-부스트 모듈 | 1 | [buck_converter_reference.stl](../reference-parts/buck_converter_reference.stl) | 실제 보드 외곽·홀, 가장 높은 부품, 단자·조정 나사 접근 공간 |
| VL53L1X 모듈 | 1 | [VL53L1X_reference.stl](../reference-parts/VL53L1X_reference.stl) | PCB 외곽·홀, 광학창 중심·방향, 헤더·케이블 공간 |
| 나사·너트·스페이서·인서트 | 설계에 따라 | [M3_spacer_10mm_reference.stl](../reference-parts/M3_spacer_10mm_reference.stl)는 스페이서 예시만 제공 | 실제 길이·외경·나사머리·너트 폭, 공구 공간, 인서트 제조사 권장 홀 |
| 전원 스위치·커패시터·커넥터·배선 | 스위치는 구성에 따라, 커패시터 1개, 나머지 필요량 | 전용 모델 없음 | 스위치 패널 구멍·걸쇠, 커패시터 지름·높이, 플러그 삽입·분리와 선의 굽힘 공간 |

위 STL은 모두 **배치 참고용**이다. `wheel_70mm`라는 이름이 실제 바퀴가 70 mm라는 뜻은 아니며, `buck_converter` 파일도 특정 벅-부스트 제품의 CAD가 아니다. 나사·보드·센서는 명칭이 같아도 판매 옵션에 따라 크기와 홀 위치가 달라진다. 특히 TB6612FNG·VL53L1X·BMI270은 **칩 단품 CAD가 아니라 구매한 모듈 전체의 CAD**가 필요하다.

기존 [N20withEncoder_mount.stl](../stl/N20withEncoder_mount.stl)은 출력 마운트 후보이며 모터 참조 모델과 다르다. 실측 적합성을 확인하기 전에는 그대로 양산하지 않는다.

### 다른 버전과 조종기

| 대상 | 추가하거나 교체할 참조 부품 | 현재 자료 |
|---|---|---|
| v0~v3 TT 2륜 차량 | TT 모터 2개, 호환 바퀴 2개, 볼 캐스터 1개. 드라이버는 1개 | 정확한 부품별 CAD 없음. v4 N20 모델을 크기만 늘려 사용하지 않는다 |
| v2 조이스틱 조종기 | 차량과 별도의 ESP32 1개, 조이스틱 1개, 정지 버튼 1개, USB 플러그. 보조배터리를 내장할 경우 그 외곽도 포함 | [케이스 상판](../stl/controller-top.STL)·[하판](../stl/controller-bottom.STL)은 케이스 후보이며 내부 부품의 실측 CAD가 아니다 |
| v3 제스처 조종기 | ESP32·정지 버튼·USB 공간과 BMI270 1개. 기본 입력은 조이스틱에서 BMI270으로 교체 | BMI270 참조 CAD 없음. 보드 축 방향도 표시하여 코드의 좌표계와 맞춘다 |

## 2. 어떤 파일을 확보하고 저장할까

| 목적 | 권장 파일 | 쓰는 방법과 한계 |
|---|---|---|
| 구매 부품의 3D 형상 확보 | **STEP / STP** | 정확한 제조사·모델·보드 리비전의 파일을 우선 확보한다. 면·축·홀을 기준으로 배치하기 좋지만 실물과 대조해야 한다. 원래 CAD의 스케치·설계 이력을 담는 형식은 아니다 |
| 실측해서 직접 만든 부품·마운트 수정 | **Fusion 원본 / F3D** | 스케치와 파라미터를 연결한 상태로 저장한다. 외부 참조가 있는 조립체의 로컬 내보내기는 F3Z 묶음이 될 수 있다 |
| 외곽 배치 참고, 기존 출력물 참조 | **STL / 3MF** | 메시로 삽입한다. STEP이나 파라메트릭 원본과 같지 않다. STL을 STEP으로 변환해도 누락된 실측 정보나 설계 이력이 생기지 않는다 |
| 2D 외곽·홀 패턴 | DXF | 실제 축척과 치수 확인 후 스케치로 삽입한다. 높이는 별도로 입력한다 |
| 치수 도면·사진 기록 | PDF, PNG, JPG | PDF 도면을 읽고 치수를 입력한다. 이미지를 Canvas로 쓰는 경우 알려진 길이로 Calibrate하되 사진 원근과 왜곡을 고려한다 |
| 완성한 출력 부품을 슬라이서로 전달 | **3MF 또는 STL** | 차체·마운트 등 출력 대상만 내보낸다. 모터·보드·배터리 참조 모델은 제외한다 |

Fusion 원본과 외부 참조의 저장 형식은 [Autodesk 로컬 아카이브 안내](https://www.autodesk.com/support/technical/article/caas/sfdcarticles/sfdcarticles/How-to-make-a-local-archive-back-up-file-in-Fusion-360.html)를 참고한다.

CAD는 구매한 제품 페이지의 CAD/Download/Mechanical Drawing에서 먼저 찾는다. 없으면 판매자에게 정확한 옵션의 STEP과 치수 도면을 요청하거나 직접 측정해 단순 모델을 만든다. 외형이 비슷한 다른 제품의 모델은 임시 배치용으로만 분류한다.

기성품 모델에는 작은 저항·문자·기어 내부를 모두 재현할 필요가 없다. **외곽, 장착면, 체결홀, 축, 돌출부, 커넥터 위치**를 정확하게 만들면 장착 설계에 사용할 수 있다.

Fusion에서는 원본을 프로젝트에 저장하고, 로컬 파일 이름은 `정확한모델명_리비전.step`, `정확한모델명_measured.f3d`처럼 출처와 상태를 구분한다. 측정 전에는 `measured`로 이름 붙이지 않는다. 제작 중간 파일·측정 사진·시험 출력 기록은 로컬 `_local`에 보관한다. 수업에 사용할 최종 설계 파일은 `hardware`에 두고 README에 연결한다. 외부 CAD의 공개 재배포 가능 여부는 해당 출처의 조건을 확인한다.

## 3. 실측 기록 방법

캘리퍼스의 영점을 확인하고 단위를 mm로 통일한다. 보드의 맨바닥 크기만 재지 말고 **핀헤더·커넥터가 장착된 실제 사용 상태**도 측정한다. 아직 확인하지 못한 값은 빈칸 또는 ‘미측정’으로 남긴다.

부품마다 아래 양식을 복사해 기록한다. 기준면·원점을 먼저 정해야 홀 좌표를 나중에 다시 사용할 수 있다.

| 기록 항목 | 입력 |
|---|---|
| 부품 이름 / 판매처 / 모델 / 옵션 / 리비전 | 미기록 |
| 실측일 / 측정 도구 / 표본 번호 | 미기록 |
| 치수 출처 구분: 실측 / 해당 모델 도면 / 임시값 | 미기록 |
| 장착 기준면 / 원점 / X·Y·Z 방향 | 미기록 |
| 본체 길이 L / 폭 W / 높이 H (mm) | 미측정 |
| 장착면 아래 돌출 높이 / 위쪽 최대 높이 (mm) | 미측정 |
| 홀별 지름 및 중심 좌표 X·Y (mm) | 미측정 |
| 축 중심 좌표·높이 / 축 지름 / D컷 / 돌출 길이 | 해당 시 미측정 |
| 케이블·커넥터 출구 위치 / 플러그 장착 후 외곽 | 미측정 |
| 조립·분해·배터리 교체·공구에 필요한 공간 | 미확인 |
| 사용 CAD 파일 / 실물과 다른 부분 | 미기록 |

모터는 축 방향을 길이로 하고 기어박스 앞면·출력축 중심을 기준으로 삼는다. PCB는 장착면과 한 모서리를 기준으로 홀 중심을 기록한다. 바퀴는 축 중심·허브 장착면을 기준으로 한다. CAD를 가져온 뒤 모델 원점이 이 기준과 다르면 기준 축·평면을 만들어 배치한다.

같은 지름의 홀 두 개는 ‘바깥쪽 끝 간 거리 − 홀 지름’ 또는 ‘안쪽 끝 간 거리 + 홀 지름’으로 중심 간격을 확인할 수 있다. 장착홀을 단순히 보드 가장자리에서 눈대중으로 배치하지 않는다.

## 4. Fusion에서 불러오기

### STEP을 확보한 경우

1. Data Panel → Upload에서 STEP/STP 파일을 올린다.
2. 변환된 부품을 열고 Inspect → Measure로 알려진 길이와 비교한다. 실측치와 차이가 나면 제품 옵션·모델 버전·누락된 커넥터 등을 확인한다.
3. 부품을 저장한다. 차체와 부품을 함께 작업할 새 디자인은 **Hybrid Design**으로 만든다. 구버전에서는 이에 해당하는 일반 Design을 사용한다.
4. Data Panel의 부품을 우클릭 → **Insert into Current Design**으로 삽입한다. 이렇게 넣은 부품은 원본과 연결된 외부 컴포넌트가 된다.

절차 출처: [Autodesk 파일 가져오기](https://help.autodesk.com/view/fusion360/ENU/?caas=caas%2Fsfdcarticles%2Fsfdcarticles%2FHow-to-import-or-open-a-file-in-Autodesk-Fusion-360.html), [다른 디자인 삽입](https://help.autodesk.com/view/fusion360/ENU/?caas=caas%2Fsfdcarticles%2Fsfdcarticles%2FHow-to-Insert-a-Component-into-the-Current-Design-in-Fusion-360.html).

### 지금 있는 STL을 쓰는 경우

1. 부품별 컴포넌트를 만들고 활성화한다.
2. **Insert Mesh**를 실행한다. Mesh → Create 또는 Insert 메뉴에서 찾을 수 있다.
3. 위 표의 STL을 선택하고 **Millimeter**로 삽입한다. STL 자체에는 단위 정보가 없으므로 크기를 반드시 확인한다.
4. 부품별로 배치한다. 크기가 실물과 다르면 중요한 외곽·축·홀을 실측으로 다시 모델링한다. 전체 메시를 비례 확대하면 홀 간격과 축 지름까지 함께 바뀌므로 실측 보정으로 간주하지 않는다.

절차 출처: [Autodesk Insert Mesh](https://help.autodesk.com/cloudhelp/ENU/Fusion-Mesh/files/MESH-INSERT-MESH.htm), [STL 단위 안내](https://help.autodesk.com/view/fusion360/ENU/?caas=caas%2Fsfdcarticles%2Fsfdcarticles%2FHow-to-insert-a-mesh-body-into-Fusion-360.html).

## 5. 실측 모델과 출력 여유를 연결하기

Modify → Change Parameters에서 아래와 같이 이름을 정한다. 아래 목록은 **새로 구성할 설계의 예시**이며 기존 생성기의 파라미터 이름과 자동으로 연결되지 않는다.

| 파라미터 예시 | 입력할 내용 |
|---|---|
| `motor_w`, `motor_h`, `motor_total_l` | 실측한 모터 외곽. 엔코더 포함 여부를 설명에 기록 |
| `shaft_d`, `shaft_l`, `shaft_center_z` | 실측한 출력축과 장착 기준면 사이 관계 |
| `bracket_hole_pitch_x`, `bracket_hole_pitch_y` | 구매 브래킷의 실제 홀 중심 간격 |
| `wheel_d`, `wheel_w`, `hub_offset` | 실측한 바퀴와 장착면 관계 |
| `battery_l`, `battery_w`, `battery_h` | 실제 사용 상태의 홀더 외곽 |
| `fit_gap_side` | 한쪽 면에 주는 끼움 여유. 실측치와 별도 관리 |
| `wire_space`, `usb_service_space` | 실제 커넥터와 케이블의 조작 공간 |
| `wall_t`, `wheelbase`, `track_width` | 출력 벽 두께, 앞뒤 축 간 거리, 좌우 바퀴 중심 간 거리라는 설계값 |

실측 부품 컴포넌트는 실제 크기로 유지하고, **마운트의 안쪽 크기에만 여유를 더한다.** 예를 들어 다음처럼 스케치 치수에 식을 입력한다.

```text
모터 받침 안쪽 폭 = motor_w + 2 * fit_gap_side
배터리 트레이 안쪽 길이 = battery_l + 2 * fit_gap_side
배터리 트레이 안쪽 폭 = battery_w + 2 * fit_gap_side
```

파라미터를 목록에 등록하는 것만으로 형상이 바뀌지 않는다. 스케치 치수·Extrude 거리·배치 기준이 해당 파라미터나 식을 실제로 참조해야 한다. [Autodesk 파라미터 작성 안내](https://help.autodesk.com/cloudhelp/ENU/Fusion-Model/files/SLD-MODIFY-CHANGE-PARAMETERS.htm).

한쪽 여유 0.20 / 0.30 / 0.40 mm를 비교하는 작은 끼움 시편을 첫 실험으로 만들 수 있다. 이는 **제안하는 시험값**이며 프린터의 보장 공차나 부품 실측치가 아니다. 통과홀·모터 고정·인서트 홀에는 각각 맞는 시험을 한다. 특히 인서트 홀은 실제 인서트 제조사 권장값에서 시작한다. 기존 생성기의 `fit_clearance = 0.4 mm`를 새 설계의 ‘한쪽 여유’와 동일하다고 가정하지 않는다.

## 6. 부품을 기준으로 차체를 만드는 순서

1. **좌표와 배치를 정한다.** X=차량 좌우, Y=앞뒤, Z=위로 통일한다. 바닥 평면과 바퀴 축선을 먼저 정하고 휠베이스·윤거·필요 지상고를 설계값으로 둔다. 기존 300×400 mm 또는 320×420 mm를 고정 조건으로 가져오지 않는다.
2. **모터·바퀴·브래킷 한 세트부터 맞춘다.** 축과 허브를 동심으로 맞추고 실제 삽입 깊이를 반영한다. 타이어가 브래킷이나 차체에 닿지 않는지, 모터 뒤 엔코더와 케이블이 들어가는지 확인한다. 구입한 금속 브래킷을 사용하면 실제 홀 패턴에 맞는 차체 어댑터를 설계하고, 출력 브래킷을 사용하면 실제 모터를 기준으로 클램프를 설계한다.
3. **확인한 세트를 네 위치에 배치한다.** Move/Copy·Align으로 초기 위치를 잡고 고정부는 Rigid Joint 등으로 관계를 정한다. 필요하면 바퀴 회전 관계를 별도로 만든다. 구매품 복제는 실제 부품의 방향 전환으로 처리하고, 비대칭 부품을 Mirror하여 실재하지 않는 반대손 부품을 만들지 않는다.
4. **배터리를 낮고 중앙 가까이에 배치한다.** 홀더를 차체에서 빼지 않고 배터리를 바꿀지, 홀더째 뺄지 정한 뒤 트레이·스트랩 슬롯·덮개 공간을 설계한다.
5. **전자부품과 서비스 공간을 놓는다.** ESP32의 USB 연결, TB6612FNG 1개의 배선, 벅-부스트의 단자, 스위치 조작, 커패시터 높이를 포함한다. 실제 장착홀이 없는 보드는 임의의 M3 홀을 뚫는 대신 가장자리 지지·클립·별도 캐리어를 설계한다. 안테나 앞 공간도 확보한다.
6. **전방 센서 위치를 확정한다.** VL53L1X의 광학창을 전방으로 향하게 하고 범퍼·차체가 시야를 가리지 않게 한다. 제품의 시야각 자료와 실제 감지 시험으로 위치를 확인한다.
7. **그 사이를 차체로 연결한다.** `CHASSIS`, `MOTOR_MOUNT`, `BATTERY_TRAY`, `ELECTRONICS_DECK`, `SENSOR_BRACKET`처럼 출력물을 별도 컴포넌트로 만든다. 장착면·홀 → 기본 판 → 보강 리브 → 필렛 순서로 설계한다. 외곽 전체보다 체결 기준을 먼저 잡는다.
8. **조립·분해와 간섭을 확인한다.** 솔리드 참조 모델과 출력물에 Inspect → Interference를 사용하고 단면·치수 측정도 확인한다. STL 외곽만 보았다고 전체 간섭 검사가 완료된 것으로 기록하지 않는다. 케이블 굽힘, USB 탈착, 드라이버 공구, 배터리 교체 공간은 별도 단순 솔리드로 표시하면 검토하기 좋다. [Autodesk 간섭 검사](https://help.autodesk.com/view/fusion360/ENU/?contextId=DESIGN-INSPECT-INTERFERENCE-CMD).
9. **브래킷 한 개와 홀 시편부터 출력한다.** 모터 고정, 엔코더 공간, 바퀴 회전, 나사 체결을 실물로 확인하고 치수를 보정한다. 이후 전체 차체를 출력한다. 프린터의 실제 출력 가능 영역과 브림 공간을 확인하여 크기·분할·접합 위치를 정한다.
10. **원본과 출력 파일을 구분해 저장한다.** 수정 가능한 Fusion 원본을 저장하고 출력 대상 컴포넌트만 우클릭 → Save As Mesh → 3MF 또는 STL로 내보낸다. STL은 mm로 저장하고 슬라이서에서 알려진 길이를 한 번 더 확인한다. [Autodesk 메시 내보내기](https://help.autodesk.com/cloudhelp/ENU/Fusion-Mesh/files/MESH-SAVE-AS-MESH.htm).

## 7. 기존 생성기는 어디까지 사용할까

- [참조 조립 생성기](../reference-parts/RC_CAR_Reference_Assembly_Builder.py): 빠른 배치 개념 확인용이다. 일부 외곽은 파라미터로 연결되지만 부품 위치와 여러 치수는 고정 수치다. 예를 들어 ESP32·TB6612 외곽 생성은 등록된 길이·폭 파라미터 대신 수치를 직접 사용한다. 현재 코드에서 모터 축과 바퀴 축 방향도 실제 동축 조립을 표현하지 않으므로 완성된 구동계 조립체로 사용하지 않는다.
- [기본 차체 생성기](RC_CAR_Chassis_Frame_Builder.py): 차체 형태를 살펴보는 시작점이다. 기본 외곽은 실측 부품에서 산출한 결과가 아니다.
- [장착 프레임 생성기](RC_CAR_Mounting_Frame_Builder.py): 장착 구조의 참고안이다. 실측한 브래킷 홀·모터 축·배터리 크기에 맞춰 검토해야 한다.
- [로버 생성기](RC_CAR_Rover_Chassis_Builder.py): 큰 차체와 상부 데크 구조의 참고안이다. 실제 프린터 크기와 필요한 부품 배치에서 차체 크기를 다시 결정한다.

이번 실습의 첫 결과물은 **실측한 모터 + 바퀴 + 브래킷 한 세트와, 시험 출력한 마운트 한 개**로 잡는다. 이 체결이 맞은 뒤 나머지 부품 배치와 전체 차체로 진행한다.

[전체 부품표](../../parts/BOM.md) · [v4 구매 규격](../../parts/purchasing/v4.md) · [v4 실습](../../v4/README.md) · [하드웨어 안내](../README.md)
