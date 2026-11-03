#!/usr/bin/env bash
# Optional: create the initial teamwork issues after the repository exists.
set -euo pipefail
REPOSITORY='Yengner/os-project2-password-cracking'
if ! command -v gh >/dev/null; then
    echo 'Install GitHub CLI first: brew install gh' >&2
    exit 1
fi
if ! gh repo view "$REPOSITORY" >/dev/null 2>&1; then
    echo "GitHub repository does not exist or is inaccessible: $REPOSITORY" >&2
    exit 1
fi
create_issue() {
    local title="$1"
    local body="$2"
    if gh issue list --repo "$REPOSITORY" --state all --limit 100 --json title --jq '.[].title' | grep -Fxq "$title"; then
        echo "Already exists: $title"
    else
        gh issue create --repo "$REPOSITORY" --title "$title" --body "$body"
    fi
}
create_issue 'P0: Verify baseline and document behavior (Member 3)' 'Build and run the unmodified baseline on each machine. Verify expected sample output and document edge cases, including unmatched hashes and duplicate inputs. Acceptance: all 3 team members can run make test. See docs/TESTING.md and docs/ISSUE_BACKLOG.md.'
create_issue 'P1: Improve hash matching and lookup (Member 2)' 'Explore efficient target comparisons and lookup design inside src/hash.c only. Preserve the original output. Submit a PR with tests and timing evidence. See docs/ARCHITECTURE.md.'
create_issue 'P2: Implement a bounded pthread worker pool (Team Lead)' 'Partition the work safely in src/hash.c, never exceeding 12 simultaneous threads including main. Benchmark 1, 2, 3, 4, and possibly more workers; verify deterministic output. Submit a reviewed PR.'
create_issue 'P3: Determinism, edge cases, and resource safety (Member 3)' 'Ensure output order, duplicate handling, first-match semantics, resource cleanup, and no data races remain baseline-equivalent. Add reproducible test evidence in the PR.'
create_issue 'P4: Benchmark and select the fastest correct implementation (All)' 'Run repeated full datasets, record 5-trial medians, compare with unmodified baseline, and choose worker count based on measured throughput and correctness. See docs/TESTING.md.'
create_issue 'P5: Final code review and Gradescope submission (Lead + reviewer)' 'Verify only src/hash.c changed among starter files, add group member details/comments, run make test, prepare dist/project2.zip, inspect its contents, and list all contributors on Gradescope.'
create_issue 'P6: Record presentation and complete peer reviews (All)' 'Produce an 8–12-minute video with names and photos on first slide, roles, approach, demo, results, team process. Upload directly to Canvas under 200 MB and complete peer reviews.'
echo 'Initial issue backlog created (or already present).'
