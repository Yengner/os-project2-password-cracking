# Architecture / team decision log

Use this to document choices for the presentation, and avoid making performance claims without data.

| Date | Decision | Options | Reason/evidence | Members involved |
|---|---|---|---|---|
| TBD | Choose worker count | 1, 2, 3, 4, ... ≤11 | Benchmark medians on four-core machine | TBD |
| TBD | Match lookup representation | Hex string scan vs binary scan vs indexed lookup | Correctness and speed as target count varies | TBD |
| TBD | Duplicate targets handling | Direct result map vs preserving baseline semantics | Compare official/baseline outputs | TBD |
| TBD | Result coordination | Mutex vs worker-local results + merge | Race safety, correctness, and timing | TBD |
