# Architecture and optimization strategy

## Existing implementation (read before editing)

1. `main.c` calls `crack_hashed_passwords(password_file, hashes_file, output_file)` and prints elapsed seconds.
2. `hash.c` loads target hashes in input order.
3. It reads one password token at a time (`fscanf`), computes MD5, SHA1, SHA256, SHA512 through the supplied wrappers, formats **the first 16 digest bytes as 32 lowercase hex characters**, then compares against each unmatched target.
4. For a match, it records `password:algorithm`, then prints one output line per target, or `not found`.

**Critical:** The implementation's exact duplicate behavior matters. It breaks after assigning the **first unmatched target** for a given candidate/algorithm and processes candidates sequentially. Do not assume that a generic parallel first-match map reproduces behavior for repeated target hashes or repeated dictionary entries; verify against the original.

## Likely expensive work

- Four digest computations per candidate password.
- Per-hash repeated `sprintf` formatting of 16 bytes to hexadecimal.
- Scanning every unmatched target digest for every hash; runtime may scale with number of target hashes.
- Allocation and free from wrapper calls for each digest.

The provided test has **999,998** password lines and **16** target hashes. Hidden datasets will differ; avoid workload-specific assumptions.

## Proposed implementation layers (hypotheses to benchmark)

### A. Lookup and digest representation (Teammate 2)

Parse each 32-character target hexadecimal hash once to 16 bytes. Compare binary digest prefixes directly (`memcmp`) rather than formatting each computed digest with `sprintf` repeatedly. A compact hash table or sorted lookup could reduce comparisons for many target hashes, but small inputs might favor direct scans. Preserve original hash positions and duplicate targets. Benchmark both choices.

### B. Parallel candidate processing (Team Lead)

Load or partition password records (do not exceed memory budget), distribute independent candidate ranges across a **bounded pthread worker pool**, and compute the four candidate digests without shared mutable cryptographic contexts. Select worker counts based on measurements on 4 cores (start at 1, 2, 3, 4; optionally 6/8). No more than 11 workers plus main. Use mutexes or thread-local findings and a deterministic merge; no uncontrolled races.

### C. Deterministic result selection, input/output and robustness (Teammate 3)

Ensure final output follows *original target order* and chooses matches with the same candidate/algorithm precedence and duplicate semantics as the baseline. Investigate ordering keys `(dictionary_position, algorithm_index)` and whether special handling is needed for duplicate targets. Validate empty/no-match, repeated values, multiple candidate matches, and valid token size. Preserve failure behavior and release allocations.

## Concurrency safety rules

- Candidate input can be parallelized; results cannot be accepted on `first thread wins` timing because scheduling is nondeterministic.
- Do not let a worker write an output file. Main writes final lines in original target order after all workers finish.
- Keep worker-local scratch buffers and contexts. Document ownership of every allocated pointer and lock.
- If reading the same `FILE *` from multiple workers, use proper coordination; better to have clear input partition ownership.
- `pthread_create`, `pthread_join`, mutexes, semaphore counts, and malloc return values should be checked appropriately.
- Threads in execution includes main. Never launch 12 workers simultaneously.

## Decisions to confirm experimentally

1. In-memory indexed dictionary versus producer/consumer batches (memory and overhead).
2. Number of worker threads for 4 core system.
3. Binary linear scan versus indexed target hash lookup based on target count.
4. How repeated hash targets, duplicate candidate passwords and algorithm collisions reproduce exact baseline behavior.
5. Whether startup, parsing, hash computation, and output each dominate in different datasets.

**Avoid** hardcoding provided hash answers, assuming fixed input sizes, calling newer crypto APIs instead of the provided wrapper functions without checking assignment restrictions, or modifying protected files.
