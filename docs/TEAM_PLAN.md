# Three-person implementation plan

## Roster (fill in before coding and submitting)

| Role | Name | GitHub username | USF NetID | Primary ownership |
|---|---|---|---|---|
| Team Lead / Member 1 | TBD | TBD | TBD | Thread model, integration, reviews, submission |
| Member 2 | TBD | TBD | TBD | Hash lookup and binary matching, correctness tests |
| Member 3 | TBD | TBD | TBD | Deterministic results, I/O/memory improvements, benchmarks and release QA |

Each member contributes C implementation in `hash.c` **and** reviews/testing. Update this table as soon as the team agrees.

## Workflow by phase (relative order; add actual due dates from Canvas)

| Phase | Primary | Other members | Exit criterion |
|---|---|---|---|
| 0 — Set up | Lead | Everyone clones, runs test | All three can build and verify baseline |
| 1 — Specify behavior | Member 3 | Lead and Member 2 review | Ground-truth edge cases and determinism requirements documented |
| 2 — Fast matching | Member 2 | Member 3 tests; Lead reviews | A standalone PR improves lookup/hash conversion, exact results maintained |
| 3 — Parallel execution | Lead | Member 2 code reviews; Member 3 benchmarks | Bounded pthread pool; clean builds, no data races, exact results |
| 4 — Determinism + hardening | Member 3 | Members 1/2 test and review | Duplicate/ordering cases, resource cleanup, outputs verified |
| 5 — Tune/verify | Everyone | Each logs tests and contributions | Compare 1/2/3/4+ workers, multiple inputs, stable speed & correctness |
| 6 — Final deliverables | Lead coordinates | All present and peer review | ZIP, Gradescope roster, video, Canvas submission, peer reviews |

### Collaboration during single-file edits

1. Agree on internal structures/function boundaries in an issue or short call.
2. **Only one implementation PR editing the same region of `hash.c` is active for integration at a time.** Others can work on tests/analysis/docs or on unconflicted helper functions after coordination.
3. Member 2's lookup patch should merge before Lead's pthread patch. Member 3 then merges the result-order/memory hardening patch, avoiding divergent branches.
4. Every code PR has at least one **other** member's review and logs the timings and correctness evidence.
5. If merging changes produces a regression, revert or fix before further optimization.

## Small, tangible responsibilities

**Lead:** Thread count and worker lifecycle; task partitioning; mutex/thread-local selection; merge oversight; verify only `hash.c` changed; ensure group included in Gradescope.

**Member 2:** Fast target representation and matching implementation; tests that compare against baseline; microbenchmarks for small and large numbers of targets; explain tradeoffs in presentation.

**Member 3:** Deterministic merging and original-order output implementation; memory and file handling (within `hash.c`); develop regression matrix; run repeated Linux benchmarks; collect demo output and help package/review.

## Decision-making and conflicts

Design by short written proposal → compare using exact output and multiple runtime trials → two-peer discussion → choose reproducible, simplest safe solution. If no agreement, log alternatives/trade-offs in `docs/DECISION_LOG.md`; Lead coordinates a decision with input from both peers. If someone's task becomes larger, redistribute testing/review obligations so each contribution is roughly equal.

## Evidence of teamwork

Each teammate logs dated commits/PRs, test ownership, major decisions, and presentation contributions in `docs/WORK_LOG.md`. Present genuine work; do not invent responsibilities.
