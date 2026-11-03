#!/usr/bin/env bash
# Development-only benchmark: verify correctness after *every* timed trial.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TRIALS="${1:-5}"
if ! [[ "$TRIALS" =~ ^[1-9][0-9]*$ ]]; then
    echo "Usage: $0 [positive-number-of-trials]" >&2
    exit 2
fi
cd "$ROOT/src"
make
printf 'Trial,ProgramSeconds\n'
for ((i=1; i<=TRIALS; i++)); do
    elapsed="$(./project2 data/common-passwords.txt data/hashes.txt output.txt)"
    diff -q data/expected.txt output.txt > /dev/null
    printf '%d,%s\n' "$i" "$elapsed"
done
