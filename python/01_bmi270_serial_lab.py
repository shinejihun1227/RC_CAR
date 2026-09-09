"""BMI270 Arduino 시리얼 로그를 읽어 Roll/Pitch와 제스처 명령을 계산한다.

입력 펌웨어: v3/firmware/00_bmi270_read/00_bmi270_read.ino
출력 형식:
accel_g x=0.000 y=0.000 z=1.000 gyro_dps x=0.00 y=0.00 z=0.00
"""

from __future__ import annotations

import argparse
import csv
import math
import re
import sys
import time
from pathlib import Path

try:
    import serial
except ModuleNotFoundError as exc:  # pragma: no cover - 실행 환경 안내
    raise SystemExit("pyserial이 필요합니다. python -m pip install -r python/requirements.txt") from exc


LINE_RE = re.compile(
    r"accel_g\s+x=(?P<ax>-?\d+(?:\.\d+)?)\s+"
    r"y=(?P<ay>-?\d+(?:\.\d+)?)\s+"
    r"z=(?P<az>-?\d+(?:\.\d+)?)\s+"
    r"gyro_dps\s+x=(?P<gx>-?\d+(?:\.\d+)?)\s+"
    r"y=(?P<gy>-?\d+(?:\.\d+)?)\s+"
    r"z=(?P<gz>-?\d+(?:\.\d+)?)"
)


def parse_line(line: str) -> dict[str, float] | None:
    match = LINE_RE.search(line)
    if not match:
        return None
    return {key: float(value) for key, value in match.groupdict().items()}


def calculate_angles(sample: dict[str, float]) -> tuple[float, float]:
    ax, ay, az = sample["ax"], sample["ay"], sample["az"]
    roll = math.degrees(math.atan2(ay, az))
    pitch = math.degrees(math.atan2(-ax, math.sqrt(ay * ay + az * az)))
    return roll, pitch


def gesture_command(roll: float, pitch: float, dead_zone: float) -> str:
    if abs(pitch) < dead_zone and abs(roll) < dead_zone:
        return "STOP"
    if pitch > dead_zone:
        return "FORWARD"
    if pitch < -dead_zone:
        return "BACKWARD"
    return "RIGHT" if roll > 0 else "LEFT"


def main() -> int:
    parser = argparse.ArgumentParser(description="BMI270 시리얼 분석 실습")
    parser.add_argument("--port", required=True, help="예: COM5 또는 /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--dead-zone", type=float, default=8.0)
    parser.add_argument("--csv", type=Path, help="분석 결과를 저장할 CSV 경로")
    args = parser.parse_args()

    writer = None
    csv_file = None
    if args.csv:
        csv_file = args.csv.open("w", newline="", encoding="utf-8")
        writer = csv.writer(csv_file)
        writer.writerow(["time_s", "ax_g", "ay_g", "az_g", "gx_dps", "gy_dps", "gz_dps", "roll_deg", "pitch_deg", "command"])

    print("BMI270 분석 시작. 종료하려면 Ctrl+C를 누르세요.")
    try:
        with serial.Serial(args.port, args.baud, timeout=1) as port:
            time.sleep(2)
            while True:
                line = port.readline().decode("utf-8", errors="replace").strip()
                sample = parse_line(line)
                if sample is None:
                    continue
                roll, pitch = calculate_angles(sample)
                command = gesture_command(roll, pitch, args.dead_zone)
                elapsed = time.monotonic()
                print(f"roll={roll:6.1f}° pitch={pitch:6.1f}° command={command:8s} | {line}")
                if writer:
                    writer.writerow([
                        f"{elapsed:.3f}", sample["ax"], sample["ay"], sample["az"],
                        sample["gx"], sample["gy"], sample["gz"], roll, pitch, command,
                    ])
                    csv_file.flush()
    except KeyboardInterrupt:
        print("\nBMI270 실습을 종료합니다.")
    except serial.SerialException as exc:
        print(f"시리얼 포트를 열 수 없습니다: {exc}", file=sys.stderr)
        return 1
    finally:
        if csv_file:
            csv_file.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
