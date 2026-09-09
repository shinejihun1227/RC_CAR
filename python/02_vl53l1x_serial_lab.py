"""VL53L1X Arduino 시리얼 거리값을 읽어 안전 상태를 표시한다.

입력 펌웨어: v1/firmware/01_distance_read/01_distance_read.ino
출력 형식: distance_mm=350
"""

from __future__ import annotations

import argparse
import csv
import re
import sys
import time
from pathlib import Path

try:
    import serial
except ModuleNotFoundError as exc:  # pragma: no cover
    raise SystemExit("pyserial이 필요합니다. python -m pip install -r python/requirements.txt") from exc


DISTANCE_RE = re.compile(r"distance_mm=(?P<distance>\d+)")


def main() -> int:
    parser = argparse.ArgumentParser(description="VL53L1X 거리·안전 판정 실습")
    parser.add_argument("--port", required=True)
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--warning-mm", type=int, default=500)
    parser.add_argument("--stop-mm", type=int, default=200)
    parser.add_argument("--csv", type=Path)
    args = parser.parse_args()

    writer = None
    csv_file = None
    if args.csv:
        csv_file = args.csv.open("w", newline="", encoding="utf-8")
        writer = csv.writer(csv_file)
        writer.writerow(["time_s", "distance_mm", "state"])

    print("VL53L1X 분석 시작. 종료하려면 Ctrl+C를 누르세요.")
    try:
        with serial.Serial(args.port, args.baud, timeout=1) as port:
            time.sleep(2)
            while True:
                line = port.readline().decode("utf-8", errors="replace").strip()
                if line == "timeout":
                    print("측정 시간 초과 → 안전을 위해 STOP 처리")
                    continue
                match = DISTANCE_RE.search(line)
                if not match:
                    continue
                distance = int(match.group("distance"))
                if distance <= args.stop_mm:
                    state = "STOP"
                elif distance <= args.warning_mm:
                    state = "WARNING"
                else:
                    state = "GO"
                print(f"distance={distance:4d} mm | state={state}")
                if writer:
                    writer.writerow([time.monotonic(), distance, state])
                    csv_file.flush()
    except KeyboardInterrupt:
        print("\nVL53L1X 실습을 종료합니다.")
    except serial.SerialException as exc:
        print(f"시리얼 포트를 열 수 없습니다: {exc}", file=sys.stderr)
        return 1
    finally:
        if csv_file:
            csv_file.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
