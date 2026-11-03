# Initial GitHub issue backlog

Open these issues once the repository exists. Assign each to the indicated role; include a peer reviewer. These are proposals, not yet-created GitHub issues.

## P0 — Baseline and behavior contract — Member 3

**Deliverables:** verified build on each machine, reference SHA/runtime/output, test checklist for no-match, duplicates, algorithm precedence, candidate order, hash prefix. Record results in `docs/TESTING.md` and `docs/WORK_LOG.md`. Acceptance: 3 team members confirm environment and baseline.

## P1 — Fast hash lookup and digest comparison — Member 2

**Deliverables:** implement efficient comparisons in `src/hash.c`; retain indices/duplicates; evaluate direct binary comparisons and optional lookup structure. PR includes correctness and timing evidence. Acceptance: no output differences and measurable performance impact.

## P2 — Bounded pthread worker pool — Lead

**Deliverables:** partition work using up to 11 workers plus main, synchronization, join, ownership cleanup; test worker counts including 1 and 4. Acceptance: exact output on sample and additional tests, no nondeterministic behavior, throughput benchmarked.

## P3 — Deterministic matches and robustness — Member 3

**Deliverables:** resolve earliest-match semantics, repeated target/word behavior, deterministic merge, correct output order, memory cleanup, error handling in `src/hash.c`. Acceptance: repeated runs identical and baseline-equivalent across approved test data.

## P4 — Performance evaluation and final tuning — All members

**Deliverables:** table of 5-trial medians per worker count and dataset, analysis of speedups, decisions documented. Acceptance: fastest reliable version selected (not simply most threads).

## P5 — Submission package and integrity review — Lead + second reviewer

**Deliverables:** protected files verified unchanged; proper author/NetID/description header and meaningful comments in `hash.c`; `make test` passes; `dist/project2.zip` contains `src/`; all 3 included in Gradescope submission. Acceptance: teammate review of exact ZIP contents.

## P6 — Video and peer review — All members

**Deliverables:** first slide member names/photos, team roles and real decision process, implementation explanation, live demo, correctness and runtime table, 8–12 minute recording, Canvas upload under 200 MB, separate peer reviews.
