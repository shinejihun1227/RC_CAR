"""네 개 N20 엔코더의 시리얼 펄스·RPM 로그를 분석한다.

입력 펌웨어: v4/firmware/00_encoder_read/00_encoder_read.ino
또는 v4/firmware/01_speed_pid_tune/01_speed_pid_tune.ino
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


PID_RE = re.compile(
    r"RPM\s+FL\s+(?P<fl>-?\d+(?:\.\d+)?)\s+"
    r"FR\s+(?P<fr>-?\d+(?:\.\d+)?)\s+"
    r"RL\s+(?P<rl>-?\d+(?:\.\d+)?)\s+"
    r"RR\s+(?P<rr>-?\d+(?:\.\d+)?)"
)
PULSE_RPM_RE = re.compile(r"(?P<pulses>-?\d+) pulses / (?P<rpm>-?\d+(?:\.\d+)?) RPM")


def parse_line(line: str) -> tuple[list[float], list[int]] | None:
    pid_match = PID_RE.search(line)
    if pid_match:
        rpm = [float(pid_match.group(name)) for name in ("fl", "fr", "rl", "rr")]
        return rpm, []

    values = PULSE_RPM_RE.findall(line)
    if len(values) == 4:
        pulses = [int(item[0]) for item in values]
        rpm = [float(item[1]) for item in values]
        return rpm, pulses
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description="N20 엔코더 RPM 분석 실습")
    parser.add_argument("--port", required=True)
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--csv", type=Path)
    args = parser.parse_args()

    writer = None
    csv_file = None
    if args.csv:
        csv_file = args.csv.open("w", newline="", encoding="utf-8")
        writer = csv.writer(csv_file)
        writer.writerow(["time_s", "rpm_fl", "rpm_fr", "rpm_rl", "rpm_rr", "spread_rpm"])

    print("엔코더 RPM 분석 시작. 종료하려면 Ctrl+C를 누르세요.")
    try:
        with serial.Serial(args.port, args.baud, timeout=1) as port:
            time.sleep(2)
            while True:
                line = port.readline().decode("utf-8", errors="replace").strip()
                parsed = parse_line(line)
                if parsed is None:
                    continue
                rpm, pulses = parsed
                spread = max(rpm) - min(rpm)
                print(
                    f"FL={rpm[0]:6.1f} FR={rpm[1]:6.1f} "
                    f"RL={rpm[2]:6.1f} RR={rpm[3]:6.1f} "
                    f"편차={spread:5.1f} RPM"
                )
                if pulses:
                    print(f"  pulses: FL={pulses[0]} FR={pulses[1]} RL={pulses[2]} RR={pulses[3]}")
                if writer:
                    writer.writerow([time.monotonic(), *rpm, spread])
                    csv_file.flush()
    except KeyboardInterrupt:
        print("\n엔코더 RPM 실습을 종료합니다.")
    except serial.SerialException as exc:
        print(f"시리얼 포트를 열 수 없습니다: {exc}", file=sys.stderr)
        return 1
    finally:
        if csv_file:
            csv_file.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
