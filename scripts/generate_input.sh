#!/bin/bash

project_root="$(dirname "$(dirname "$(realpath "${BASH_SOURCE[0]}")")")"
input_dir="$(realpath "${project_root}/input")"

for i in {1..3}; do /usr/bin/python3 "$project_root/scripts/generate_random.py" > "$project_root/input/random-$i.txt"; done
for i in {1..3}; do /usr/bin/python3 "$project_root/scripts/generate_locality.py" > "$project_root/input/locality-$i.txt"; done
for i in {1..3}; do /usr/bin/python3 "$project_root/scripts/generate_custom.py" > "$project_root/input/custom-$i.txt"; done
