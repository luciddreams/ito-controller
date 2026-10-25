#!/bin/sh
set -eu
cd "$(dirname "$0")"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM
g++ -std=c++11 -Wall -Wextra -Werror safety_test.cpp -o "$work/safety"
"$work/safety"
g++ -std=c++11 -Wall -Wextra -Werror -I. state_test.cpp -o "$work/states"
"$work/states"
g++ -std=c++11 -Wall -Wextra -Werror ambient_test.cpp -o "$work/ambient"
"$work/ambient"
g++ -std=c++11 -Wall -Wextra -Werror -I. defaults_test.cpp -o "$work/defaults"
"$work/defaults"

g++ -std=c++11 -Wall -Wextra -Werror telemetry_test.cpp -o "$work/telemetry"
"$work/telemetry" > "$work/telemetry.json"
python3 -c 'import json,sys; d=json.load(open(sys.argv[1])); assert d["sequence"]==42 and d["input_a"] is None and len(d["temperatures"])==5; print("PASS: telemetry JSON nulls, shape and buffer limits")' "$work/telemetry.json"
g++ -std=c++11 -Wall -Wextra -Werror -I. wifi_lock_test.cpp -o "$work/wifi_lock"
"$work/wifi_lock"
