# Testing and benchmarking

## 1. First run: untouched starter

```bash
cd src
make
make test
```

The provided `Makefile` writes `output.txt` and compares it to `data/expected.txt`. The initial baseline **passed** in this workspace; the program printed **4.047 seconds** on Debian Linux, GCC 14.2.0, with the starter test. Your Ubuntu/Gradescope times will differ.

## 2. After each PR

```bash
cd src
make
make test
# Repeated execution: compare exact output each time, not just approximate equality
./project2 data/common-passwords.txt data/hashes.txt output.txt
diff -u data/expected.txt output.txt
```

From repository root:

```bash
./scripts/benchmark.sh 5
```

This runs the same starter input five times, validates output on every trial, and prints the raw runtime numbers. Never benchmark one implementation with a different input or hardware.

## 3. Correctness matrix

Use the downloadable official test cases if available to the team (do not copy answers into code). Also build small local test cases with a *known original-baseline output*; compare byte for byte:

| Case | Why |
|---|---|
| Starter test | Sanity: expected 16 lines, mixed algorithms, and not-found |
| Zero matching targets | No false matches or early exit behavior changes |
| One hash only | Avoid assumptions about many hashes |
| Many hashes | Check scaling and lookup correctness |
| Duplicated target hash rows | Preserve subtle baseline assignment behavior |
| Repeated candidate passwords | Verify original first-match/duplicate behavior |
| Same target potentially matches multiple candidates | Preserve original sequential precedence |
| SHA256/SHA512 truncated to first 16 bytes | Critical digest representation detail |
| Mixed case hashes/odd length if present in official tests | Avoid accidentally broadening/narrowing baseline behavior |
| Multiple runs and worker counts | Detect nondeterministic output/races |
| Long candidates up to documented 255 chars | Buffer correctness and memory limits |

Do not assume all edge-case input forms are valid. The reference code uses whitespace-token input and fixed-size arrays; build tests that match the instructor's actual expectations, and document any undefined/unsupported input behavior.

## 4. Performance method

- Use identical inputs, machine, and invocation for baseline/optimization.
- Benchmark **1, 2, 3, 4** worker threads and any carefully justified additional counts (maximum 11 workers).
- Run each configuration at least **5 times**; record median rather than single best time.
- Compare `speedup = baseline_median / optimized_median`.
- Benchmark at least small, medium, large dictionary and varying target counts when available; note speed/memory trade-offs.
- Ensure all tests produce exactly the same output file on every repetition. A fast wrong answer earns no credit.
- Pay attention to malloc overhead, input I/O, synchronization cost, thread creation, digest hashing, and hash lookup.

Use this table for each experiment:

| Date | Machine / OS | Dataset | Workers | Trial times (s) | Median (s) | Speedup | Correct? | Git SHA |
|---|---|---|---:|---|---:|---:|---|---|
| TBD | Ubuntu 22.04 / 4 cores | starter | 1 | TBD | TBD | 1.00× baseline | TBD | TBD |

The rubric's 4.3× objective is relative to a separate reference and grading environment, so **do not promise** a score from local measurements.

## 5. CI limitations

GitHub Actions only verifies the provided sample test on Ubuntu 22.04. It is **not** a full Gradescope replacement and does not guarantee race freedom or performance points. Capture official test and presentation evidence separately.
