"""TB6612FNG·N20 모터의 시리얼 안전 주행 실습.

입력 펌웨어: v1/firmware/02_safety_stop/02_safety_stop.ino
이 펌웨어는 f/b/l/r/s 문자를 받아 모터를 제어하고, VL53L1X가 가까운
장애물을 감지하면 전진 명령을 차단한다.
"""

from __future__ import annotations

import argparse
import sys
import time

try:
    import serial
except ModuleNotFoundError as exc:  # pragma: no cover
    raise SystemExit("pyserial이 필요합니다. python -m pip install -r python/requirements.txt") from exc


VALID_COMMANDS = {"f", "b", "l", "r", "s"}


def main() -> int:
    parser = argparse.ArgumentParser(description="TB6612FNG 모터·거리 안전 정지 실습")
    parser.add_argument("--port", required=True)
    parser.add_argument("--baud", type=int, default=115200)
    args = parser.parse_args()

    print("f 전진 | b 후진 | l 좌회전 | r 우회전 | s 정지 | q 종료")
    try:
        with serial.Serial(args.port, args.baud, timeout=0.1) as port:
            time.sleep(2)
            while True:
                command = input("명령> ").strip().lower()
                if command == "q":
                    port.write(b"s")
                    print("정지 명령을 보내고 종료합니다.")
                    break
                if command not in VALID_COMMANDS:
                    print("f, b, l, r, s 중 하나를 입력하세요.")
                    continue
                port.write(command.encode("ascii"))
                time.sleep(0.05)
                while port.in_waiting:
                    print(port.readline().decode("utf-8", errors="replace").strip())
    except KeyboardInterrupt:
        print("\n종료 전 정지 명령을 보내지 못했을 수 있습니다. 차량 전원을 확인하세요.")
    except serial.SerialException as exc:
        print(f"시리얼 포트를 열 수 없습니다: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
