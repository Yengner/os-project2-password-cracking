#!/usr/bin/env bash
# Run this locally, with GitHub CLI authenticated, to create the requested public repo.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
REPOSITORY="Yengner/os-project2-password-cracking"
if ! command -v gh > /dev/null; then
    echo 'GitHub CLI is required. On a Mac: brew install gh' >&2
    exit 1
fi
if ! gh auth status > /dev/null 2>&1; then
    echo 'First sign in with: gh auth login' >&2
    exit 1
fi
if ! command -v git > /dev/null; then
    echo 'git is required.' >&2
    exit 1
fi
if [ ! -d .git ]; then
    git init -b main
fi
git add .
if ! git diff --cached --quiet; then
    git commit -m 'Initialize Operating Systems Project 2 team repository'
fi
if git remote get-url origin > /dev/null 2>&1; then
    echo 'This folder already has an origin remote; inspect it before creating another.' >&2
    exit 1
fi
gh repo create "$REPOSITORY" --public --source=. --remote=origin --push
echo "Created public repo: https://github.com/$REPOSITORY"
echo 'Next: Settings > Collaborators and teams > Add people'
echo 'Important: the assignment warns against sharing solutions publicly; consider switching to Private before committing optimization work.'
