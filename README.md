# Project 2 — Two-part implementation and handoff

**Only the first half is implemented. This is not the finished project.** The current code runs in one thread. The second half must add pthreads, tune performance, and complete final testing before submission. This splits implementation between the two active contributors; the assignment's group-size requirements still apply.

## Work split

| Owner | Work | Status |
| --- | --- | --- |
| Me — first half | Target loading, hash lookup table, duplicate handling, ordered matching, output, cleanup, and helper functions for the threading handoff in `src/hash.c`. Provide the local test script. | Implemented; locally checked against the starter. |
| Teammate — second half | Replace the serial password loop with a fixed pthread pool and bounded batches, preserve result order, tune performance, run the local Gradescope checks and all supplied course datasets on Linux, fill in author details, and prepare/submit the final ZIP. | Still to do. |

Everyone contributing should review the final code and explain their work in the video. Peer reviews remain individual submissions.

## Architecture: what exists and what to add

```text
Current first half:
load_targets → read one password → compute_password_hashes
             → apply_password_hashes → repeat → write_results → cleanup

Teammate's second half:
load_targets → read a bounded batch in password order
             → fixed workers run compute_password_hashes on separate slots
             → main waits for completion
             → main runs apply_password_hashes in password order
             → reuse batch → repeat → stop/join workers → output and cleanup
```

Implemented in `src/hash.c`:

- `load_targets`: stores output rows in original order and builds a chained hash table. Reverse insertion preserves the input order of duplicate targets. Hash collisions use full-key comparisons.
- `compute_password_hashes`: computes MD5, SHA1, SHA256, and SHA512 for one password, keeps the first 16 bytes as 32 lowercase hex characters, and frees the returned digest buffers. It has no shared mutable state; each worker must receive its own output slot.
- `apply_password_hashes`: looks up matching targets without scanning the entire target list. It assigns and removes only the first unmatched row per digest event, preserving the starter's duplicate behavior. **Call this only from the main thread, in password input order.** It processes algorithms in MD5 → SHA1 → SHA256 → SHA512 order.
- `write_results` and cleanup: preserve target row order, `password:ALGORITHM` output, and `not found` lines, and free row/table allocations.

The marked serial loop in `crack_hashed_passwords` is the integration point. Reading and computing hashes currently happen sequentially; pthreads have not been added.

## Handoff for the teammate or their AI agent

Use this as the remaining task:

> Complete only the second half of this repository's Project 2. Read `src/hash.c` and keep its indexed lookup and ordered duplicate matching. Replace the marked serial password loop with bounded batches and a fixed pthread pool. Each batch slot must retain its password and its four computed hashes. Workers call `compute_password_hashes` using private slots. The main thread must wait until the batch is complete, then call `apply_password_hashes` for each slot in original password order before reusing batch storage. Use mutexes/condition variables or semaphores for work handoff and completion; handle partial thread creation, shutdown, joins, and cleanup without hangs or races. Start with 3 workers plus main, benchmark 4 workers too, and never exceed 12 total simultaneous threads, including main. Keep memory bounded for large inputs. Preserve whitespace-separated input, passwords up to 255 characters, all four algorithms, duplicate behavior, and exact output. Do not replace deterministic input order with worker completion order. Only change `src/hash.c` among starter source/build files; keep the Makefile, compiler flags, `main.c`, both headers, and `hash_functions.c` unchanged. Use permitted pthread/semaphore and standard C facilities. Fill in real group/authors/NetIDs at the top of `hash.c` and retain meaningful comments. Run the tests below on Ubuntu 22.04/GCC 11.4, add thread/batch boundary and repeated-run checks, test every available course dataset, and benchmark before packaging. Report actual results and remaining limitations; do not claim Gradescope success until the real submission returns results.

## Teammate's final testing and submission

On Linux with Python 3, make, GCC, and OpenSSL development headers available, run from the repository:

```bash
python3 scripts/test_gradescope.py
```

The default starter archive is `starter/project2-original.zip`, identical to the provided `project2-1.zip`. To use another copy explicitly:

```bash
python3 scripts/test_gradescope.py --starter /path/to/project2-1.zip
```

The script checks that protected files are unchanged, builds original and current code with their unchanged Makefiles, rejects compiler warnings, and compares output byte-for-byte. It runs the sample plus 10 generated cases, with three current-code runs each. Cases cover all algorithms, misses, duplicate passwords/hashes, input order, whitespace, 255-character passwords, and an empty password list. It also builds and runs a freshly extracted submission ZIP. Temporary builds leave `src/` untouched.

The provided ZIP contains one sample dataset, not the separate course “all test cases” archive. Download those course cases and test each password/hash pair with this repeatable option:

```bash
python3 scripts/test_gradescope.py --case /path/to/passwords.txt /path/to/hashes.txt
```

After all checks pass, the script creates `dist/project2.zip` with `src/` and the six required source/build files. **A ZIP created now contains only the first half; do not submit it as the completed project.** Regenerate it after threading and final testing. The script does not enforce the runtime thread cap, prove race freedom, audit permitted libraries, or reproduce the Gradescope score.

Before submitting, the teammate must:

- Verify the final code on Ubuntu 22.04.3/GCC 11.4, targeting the grader's 4 CPU cores and 6 GB RAM. Repeat original/final timings on identical inputs; local sample speed is not the Gradescope speed factor.
- Test batch boundaries, repeated matching passwords across batches, repeated invocations, thread shutdown, and failure paths. Inspect for races and confirm at most 12 total threads.
- Fill in group/authors/NetIDs and review comments and indentation. Only `hash.c` may differ from the supplied source/build files.
- Regenerate and upload `dist/project2.zip`, list contributing group members in Gradescope, and review the actual grader results. Full speed credit requires 4.3× the reference speed; correctness alone does not earn speed points.
- Complete the separate video and peer-review requirements using their assignment instructions.

First-half checks passed locally on macOS against the starter; Linux verification and final threaded-code testing belong to the second half. Local checks cannot guarantee a Gradescope score.
