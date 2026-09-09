# 조종기·모터 장착 STL

| 파일 | 사용하는 실습 | 확인할 것 |
|---|---|---|
| [controller-top.STL](controller-top.STL) | v2 조이스틱 조종기 케이스 상판 | 조이스틱·버튼 구멍, ESP32 USB 공간 |
| [controller-bottom.STL](controller-bottom.STL) | v2 조종기 케이스 하판, v3 배치 검토 | 보드 고정·배선·BMI270 추가 공간 |
| [N20withEncoder_mount.stl](N20withEncoder_mount.stl) | v4 N20 모터 장착 | 모터 폭·고정홀·엔코더 PCB·축 높이 |

기존 강의 작업 폴더에 보관되어 있던 파일이다. 부품 모델명과 실제 치수를 대조하고 한 개를 시험 출력한 뒤 적용한다. 참조 부품 폴더의 외곽 STL는 이 파일들과 용도가 다르다.

외부 모델을 사용할 때는 원 출처의 사용 조건을 따른다. 기존 프로젝트의 조종기 참고 출처는 [Wallieonline ESP-NOW 조종기](https://www.wallieonline.nl/blogs/esp-now-remote-control-mini-robots.html)와 [케이스 자료](https://makerworld.com/en/models/695669#profileId-624636)이며, 참고 링크만으로 각 로컬 STL의 원 저작자나 라이선스를 단정하지 않는다.
