# Requirements and grading — source of truth: Canvas assignment

## Hard constraints

| Area | Requirement |
|---|---|
| Team | 3–4 students; this team has 3. Designate a leader and verify Canvas membership. |
| Allowed code changes | **Only `src/hash.c`** among the supplied files. Keep `main.c`, `hash.h`, `hash_functions.c`, `hash_functions.h`, and Makefile unchanged. |
| Code/libraries | C only. Use pthread, semaphore, standard C libraries and supplied project OpenSSL wrappers. No nonstandard added libraries or changed compiler flags. |
| Maximum concurrency | **12 threads total, main included**. A worker pool of at most 11 satisfies the count if no other threads are created. |
| Output | Exact agreement with baseline for all supplied and hidden tests; preserve output order, `not found`, and matching labels. |
| Grading environment | Ubuntu 22.04.3 LTS, GCC 11.4, 4 CPU cores, 6 GB RAM. |
| Packaging | `project2.zip` with folder `src/`, C source and original Makefile; builds executable `project2`. |
| Integrity | Do not share or copy other groups' code, cheat using known test answers, or make data-dependent hardcoded shortcuts. |

## Code grade (60 points)

Speeding factor is based on the *grader's reference code* and datasets, not a local wall-clock target. **50 points** are from speed:

| Speed factor | Speed credit |
|---|---:|
| ≤ 1.0× | 0 |
| 2.1× | 16.7 |
| 3.2× | 33.3 |
| ≥ 4.3× | 50 |

Intermediate factors are interpolated. Code style provides **10 points**: 2 for group/authors/NetIDs/description at top of `hash.c`; 5 for meaningful comments; 3 for indentation (4 spaces recommended). Compilation warning: **−3 points** per rubric.

## Other graded work

- Video presentation: **30 points**, between **8 and 12 minutes**, **under 200 MB**, directly submitted to Canvas, accessible/playable; ideally ~9–10 minutes. First slide includes every contributing member's **name and photo**.
- Cover overall design, key implementation details, teamwork division, approximate equality of workloads, decisions/conflicts, functional demo, correctness, and measurable speed results.
- Peer reviews are **Part 3**; consult their separate Canvas assignment.
- Add **all group members to Gradescope submission**. Failure to include a contributing member may affect their grade.
- Leader must verify exact deadlines separately for Code, Video, and Peer Reviews; none are present in the pasted description.

## Protected file policy

Documentation, CI workflows, scripts, and local test notes are **repo development helpers**, not modifications to original program source. Do **not** include these repo helpers in the final code ZIP. Only submit the allowed `src` folder.
