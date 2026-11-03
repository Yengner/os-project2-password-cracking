# Operating Systems — Project 2: Password Cracking

**Course:** Operating Systems · **Team:** 3 people · **Language:** C · **Status:** Planning / baseline verified

> **Public repository configuration requested by the team.** The instructor warns against sharing project code; keeping this repository public when you add your optimized solution may create academic-integrity risk. Make it private if required. Never commit another group's code or hidden Gradescope datasets.

## Goal

Speed up the provided password-hash matching tool **without changing its output**. The program reads candidate passwords and target hashes, tries MD5, SHA1, SHA256, and SHA512, and writes the matching password and algorithm or `not found` for each target.

Only **`src/hash.c`** may be changed among the starter files. Do not edit `main.c`, `hash.h`, `hash_functions.c`, `hash_functions.h`, or `Makefile`, and do not change compiler flags or use nonstandard libraries. Pthreads, semaphores, and standard C libraries are permitted. **At most 12 simultaneous threads including the main thread** are allowed; design for **up to 11 worker threads**, but benchmark approximately 3–4 workers on the grading machine's four CPU cores.

## Quick start (Ubuntu/Linux)

```bash
sudo apt update
sudo apt install -y build-essential libssl-dev zip unzip
cd src
make
make test
```

`make test` runs the supplied sample and compares its output against `src/data/expected.txt`. To run with custom files:

```bash
./project2 path/to/passwords.txt path/to/hashes.txt output.txt
```

The executable prints runtime (seconds) to standard output; matching results go to the output file. The supplied baseline passed locally before any optimization; it took **4.047 s** on a Debian/GCC 14.2 environment. This is **not** the Gradescope reference time and should not be treated as a score prediction.

## Repository map

```text
starter/project2-original.zip # Unmodified instructor starter package
src/                       # PROVIDED STARTER — edit only hash.c
  hash.c                    # Only permitted implementation change
  main.c, hash.h, ...       # Protected starter files
  Makefile
  data/                     # Starter sample inputs and expected output
.github/workflows/ci.yml   # Linux build + supplied test
.github/ISSUE_TEMPLATE/     # Task/bug templates
scripts/benchmark.sh       # Repeatable local correctness + runtime runs
scripts/create_github_repo.sh # Creates public repo and pushes the files
scripts/create_github_issues.sh # Creates the 7 initial teamwork issues
scripts/package_submission.sh  # Produce dist/project2.zip with src/ only
docs/REQUIREMENTS.md        # Assignment rules and grading
docs/ARCHITECTURE.md       # Baseline analysis and safe optimization ideas
docs/TEAM_PLAN.md          # Three-person tasks, phases, integration order
docs/TESTING.md            # Test matrix and benchmark protocol
docs/ISSUE_BACKLOG.md      # Issues to create and claim
docs/VIDEO_AND_SUBMISSION.md # Video, peer reviews, ZIP checklist
docs/DECISION_LOG.md       # Record architecture decisions
docs/WORK_LOG.md           # Evidence of each member's contributions
```

## How we collaborate

1. Pick a team lead, join the correct **Canvas Project 2** group, and enter names/NetIDs in `docs/TEAM_PLAN.md` and the comment header of `src/hash.c` before submission.
2. Open the initial GitHub issues from `docs/ISSUE_BACKLOG.md`; assign one primary owner per issue and include a reviewer.
3. Work on short `feature/...` or `test/...` branches. **Do not push implementation directly to `main`**.
4. Because all allowed implementation work is in **one file**, coordinate `hash.c` edits and merge in the order described in `docs/TEAM_PLAN.md`. Documentation and benchmark work can happen in parallel.
5. Each pull request must pass `make test`, state its runtime, disclose changed files, and be reviewed by another teammate.
6. When integrating, verify exact output on all available datasets, benchmark multiple times, and package from a clean working tree.

See [CONTRIBUTING.md](CONTRIBUTING.md) for commands and review rules.

## Progress checkpoints

- [x] Original starter package inspected
- [x] Original code compiled and passed starter test
- [ ] Team roster and GitHub access confirmed
- [ ] Baseline independently verified by every teammate
- [ ] Matching lookup optimization integrated and reviewed
- [ ] Worker parallelism integrated and reviewed
- [ ] Correctness and deterministic result handling validated
- [ ] Repeated benchmarks and tuning complete
- [ ] Authors/NetIDs and meaningful comments in `hash.c`
- [ ] Final `project2.zip` prepared, reviewed, and submitted
- [ ] Group presentation uploaded to Canvas
- [ ] Peer reviews completed

## Important assignment targets

- **Code:** 60 points (50 speed, 10 style). Full speed credit at **4.3×** the reference; no speed points at or below **1×**.
- **Video:** 30 points; **8–12 minutes hard limit**, ideally roughly **9–10 minutes**, directly uploaded to Canvas, file **under 200 MB**.
- **Peer reviews:** separate required submission.
- **Submission:** `project2.zip` containing `src/` and an unchanged Makefile that builds executable `project2` on Ubuntu 22.04.3/GCC 11.4.

The project due date is not in the provided description. The team leader should confirm all **three Canvas deadlines** and add them to the project issues.

## Creating the public GitHub repository

The repository package is prepared but **has not been created on GitHub**. On your computer, after extracting the ZIP:

```bash
brew install gh                  # macOS; skip if already installed
gh auth login
./scripts/create_github_repo.sh
./scripts/create_github_issues.sh   # optional: add the seven teamwork issues
```

That script creates **`Yengner/os-project2-password-cracking` as a public repository** and pushes all included files (including the original starter archive). Alternatively, create an **empty public** repo at https://github.com/new with that name and push the extracted folder manually. In GitHub, open **Settings → Collaborators and teams → Add people** to invite your two teammates by their GitHub usernames. **Academic integrity:** Your instructions say not to share code; consider making the repository private before any original solution work is pushed.
