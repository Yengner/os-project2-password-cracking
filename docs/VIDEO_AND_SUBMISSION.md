# Presentation, code submission, and peer reviews

## Code submission — Gradescope

1. Fill the top of **`src/hash.c`** with group ID, all contributing authors and NetIDs, and a brief program description. Use meaningful comments and consistent 4-space indentation; ensure no compilation warnings.
2. Confirm the **five protected** starter files and Makefile are unchanged; only `src/hash.c` changes in source.
3. Verify exact output on the starter test, downloaded official cases, and representative self-constructed cases. Rerun with different worker counts to catch nondeterminism.
4. From repo root run `./scripts/package_submission.sh` to generate **`dist/project2.zip`** containing the `src/` directory and excluding compiled binaries/output. Inspect with `unzip -l dist/project2.zip`.
5. Optionally unpack ZIP to a clean directory and rerun `cd src && make && make test`. This simulates a clean extraction.
6. Submit ZIP to Gradescope; **list all three contributing members**.

## Video — Canvas (30 points)

- Length **8–12 minutes (hard limit)**; plan around **9–10 minutes**.
- Upload video file **directly to Canvas**, not YouTube/Drive/Dropbox.
- File must be **under 200 MB**, accessible, and playable in Canvas. Check final playback and deadline.
- First slide: **full names and photos of all contributing group members**.
- Explain real design decisions, threading, data structures, synchronization, correctness/ordering, and measurable speedup.
- Show how each member contributed, whether responsibilities were roughly even, how decisions and conflicts were handled.
- Live demonstration with supplied test, exact diff, timing (if recording a live run takes too long, explain a recorded, authentic run), and a results table with baseline/optimized trial medians.

### Suggested 9–10 minute outline

| Segment | Presenter | Target duration |
|---|---|---:|
| Team names/photos and goal | Lead | 0:45 |
| Baseline and bottlenecks | Member 2 | 1:15 |
| Fast matching structures | Member 2 | 1:30 |
| Thread model and synchronization | Lead | 1:45 |
| Ordering, duplicates, correctness | Member 3 | 1:15 |
| Demo and measured results | Member 3 | 1:30 |
| Collaboration, decisions, conflicts, lessons | All | 1:00 |

Total approx. **9:00**; practice once, then adjust. Ensure each person explains what they really implemented.

## Peer reviews — Canvas Part 3

Requirements are in the separate **Project 2 - Peer Reviews** assignment; open it to confirm deadline and review format. Make individual reviews accurate and consistent with commits/PRs.

## Deadline tracker

| Deliverable | Official date/time | Responsible | Submission status |
|---|---|---|---|
| Gradescope code ZIP | **TBD — check Canvas** | Lead + reviewer | Not submitted |
| Canvas video | **TBD — check Canvas** | All | Not submitted |
| Canvas peer reviews | **TBD — check Canvas** | Each member | Not submitted |
