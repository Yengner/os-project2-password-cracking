# Contributing

This is a collaborative class assignment. Each member must be able to explain the code they contribute; keep development and test evidence in pull requests.

## Setup

```bash
git clone https://github.com/Yengner/os-project2-password-cracking.git
cd os-project2-password-cracking
cd src && make && make test
```

Ubuntu requires `libssl-dev` and GCC; use the supplied Makefile unchanged. On macOS, development is possible with OpenSSL installed, but **final testing must be on Linux**, preferably Ubuntu 22.04 with GCC 11.4.

## Branch and PR flow

```bash
git switch main
git pull
git switch -c feature/short-description
# edit permitted file, run tests
git add src/hash.c  # or only your documentation/script files
git commit -m "Optimize: explain meaningful change"
git push -u origin feature/short-description
```

Open a pull request to `main`. Ask another member to review. Prefer one conceptual change per PR and small, understandable commits.

**Single editable source file:** plan interfaces first; while one teammate is modifying `src/hash.c`, others can build test cases, collect benchmarks, review, and plan the next patch. Integrate in this order: data structure / matching, parallel execution, deterministic merging / final hardening. Rebase your branch on the latest main before modifying the same functions.

## PR acceptance checklist

- Only `src/hash.c` changed among original source/Makefile files.
- No new external code dependencies, compiler flags, or tricks that bypass work.
- Never exceed 12 threads including main; use `pthread_join` and proper synchronization.
- Exact output matches the baseline for relevant test cases; output order preserved.
- Works when targets are absent, repeated, or match multiple candidate rows.
- No data races, memory leaks or unsafe buffers introduced.
- `cd src && make test` passes on Linux; CI passes.
- Performance evidence: dataset size, worker count, repetitions, times, median, and comparison to unmodified baseline on the **same environment**.
- Meaningful comments and 4-space indentation in changed `hash.c` code.

## Do not commit

Built binary (`src/project2`), local result (`src/output.txt`), secret files, Gradescope-private datasets, copied solutions, or unrelated files. The `.gitignore` excludes common local outputs.

## How to resolve decisions and conflicts

Raise a GitHub issue or PR discussion, compare options using correctness first, then speed on reproducible tests. The three members review; when opinions differ, document the trade-off in `docs/DECISION_LOG.md`. The team lead coordinates the final integration but cannot skip another member's review.
