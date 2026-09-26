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
