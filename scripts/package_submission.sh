#!/usr/bin/env bash
# Development-only packaging helper. No non-source project files are submitted.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
command -v zip > /dev/null || { echo 'zip is required' >&2; exit 1; }
command -v unzip > /dev/null || { echo 'unzip is required' >&2; exit 1; }
mkdir -p dist
rm -f dist/project2.zip
# Explicit file list keeps generated outputs, repo docs, and build binaries out.
zip -q dist/project2.zip \
    src/Makefile src/main.c src/hash.c src/hash.h \
    src/hash_functions.c src/hash_functions.h \
    src/data/common-passwords.txt src/data/hashes.txt src/data/expected.txt
printf 'Created %s/dist/project2.zip\n' "$ROOT"
unzip -l dist/project2.zip
