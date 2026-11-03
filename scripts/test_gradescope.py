#!/usr/bin/env python3
"""Compare against the supplied starter, then package and test the submission."""

import argparse
import hashlib
import os
from pathlib import Path
import random
import re
import subprocess
import sys
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[1]
FILES = ("Makefile", "main.c", "hash.c", "hash.h", "hash_functions.c", "hash_functions.h")
ALGORITHMS = ("md5", "sha1", "sha256", "sha512")


def run(args, cwd=None):
    result = subprocess.run(
        [str(arg) for arg in args], cwd=cwd, capture_output=True,
        text=True, timeout=300,
    )
    if result.returncode:
        raise RuntimeError(f"Command failed: {args}\n{result.stdout}{result.stderr}")
    return result


def build(source):
    result = run(["make", "-B"], cwd=source)
    if re.search(r"\bwarning:", result.stdout + result.stderr, re.IGNORECASE):
        raise RuntimeError(f"Compiler warning in {source}:\n{result.stdout}{result.stderr}")


def execute(source, passwords, hashes, output):
    return run([source / "project2", passwords, hashes, output]).stdout.strip()


def digest(password, algorithm):
    return hashlib.new(algorithm, password.encode("ascii")).hexdigest()[:32]


def generated_cases(folder):
    rng = random.Random(2026)
    cases = [
        ("all-algorithms", ["alpha", "beta", "gamma", "delta"],
         [digest(p, a) for p, a in zip(["alpha", "beta", "gamma", "delta"], ALGORITHMS)] + ["0" * 32]),
        ("duplicates", ["alpha", "beta", "alpha", "alpha"],
         [digest("alpha", "md5")] * 4 + [digest("alpha", "sha256")] * 2 + [digest("beta", "sha1")]),
        ("long-and-whitespace", ["x" * 255, "a", "Z9!"],
         [digest(p, a) for p in ["Z9!", "x" * 255, "a"] for a in ALGORITHMS]),
        ("no-match", ["alpha", "beta"], ["0" * 32, "f" * 32]),
        ("empty-password-list", [], [digest("alpha", "md5")]),
    ]
    for index in range(5):
        passwords = ["".join(rng.choice("abcXYZ019!_") for _ in range(rng.randint(1, 30))) for _ in range(100 + index * 37)]
        passwords += passwords[:5]
        hashes = [digest(rng.choice(passwords), rng.choice(ALGORITHMS)) for _ in range(35)]
        hashes += hashes[:4] + [digest("absent-password", "sha512")]
        rng.shuffle(hashes)
        cases.append((f"random-{index + 1}", passwords, hashes))
    result = []
    for name, passwords, hashes in cases:
        p, h = folder / f"{name}.passwords", folder / f"{name}.hashes"
        # Include mixed whitespace and no final newline: fscanf reads tokens.
        p.write_text(" \t\n".join(passwords), encoding="ascii")
        h.write_text("\n".join(hashes), encoding="ascii")
        result.append((name, p, h))
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--starter", type=Path, default=ROOT / "starter/project2-original.zip")
    parser.add_argument("--case", nargs=2, action="append", default=[], metavar=("PASSWORDS", "HASHES"))
    args = parser.parse_args()
    destination = ROOT / "dist/project2.zip"
    with tempfile.TemporaryDirectory(prefix="project2-check-") as temporary:
        work = Path(temporary)
        baseline, candidate = work / "baseline/src", work / "candidate/src"
        baseline.mkdir(parents=True)
        candidate.mkdir(parents=True)
        with zipfile.ZipFile(args.starter) as archive:
            # Read explicit members rather than extracting arbitrary ZIP paths.
            for name in FILES:
                original = archive.read(f"src/{name}")
                current = (ROOT / "src" / name).read_bytes()
                if name != "hash.c" and current != original:
                    raise RuntimeError(f"Protected file changed: src/{name}")
                (baseline / name).write_bytes(original)
                (candidate / name).write_bytes(current)
            sample = work / "sample"
            sample.mkdir()
            for name in ("common-passwords.txt", "hashes.txt", "expected.txt"):
                (sample / name).write_bytes(archive.read(f"src/data/{name}"))
        build(baseline)
        build(candidate)
        cases = [("starter-sample", sample / "common-passwords.txt", sample / "hashes.txt")]
        cases += generated_cases(work)
        for index, (p, h) in enumerate(args.case, 1):
            cases.append((f"course-{index}", Path(p).resolve(strict=True), Path(h).resolve(strict=True)))
        for name, passwords, hashes in cases:
            expected, actual = work / "expected-output", work / "actual-output"
            original_time = execute(baseline, passwords, hashes, expected)
            if name == "starter-sample" and expected.read_bytes() != (sample / "expected.txt").read_bytes():
                raise RuntimeError("Starter does not match supplied expected.txt on this environment")
            times = []
            for trial in range(3):
                actual.unlink(missing_ok=True)
                times.append(execute(candidate, passwords, hashes, actual))
                if actual.read_bytes() != expected.read_bytes():
                    raise RuntimeError(f"Output mismatch: {name}, run {trial + 1}")
            print(f"PASS {name} (3 runs)", flush=True)
            if name == "starter-sample":
                print(f"  seconds: starter={original_time}; candidate={', '.join(times)}", flush=True)

        # Test exactly the six files that will be submitted, including a fresh build.
        package = work / "project2.zip"
        with zipfile.ZipFile(package, "w", zipfile.ZIP_DEFLATED) as archive:
            for name in FILES:
                archive.write(candidate / name, f"src/{name}")
        extracted = work / "submission"
        with zipfile.ZipFile(package) as archive:
            archive.extractall(extracted)
        build(extracted / "src")
        output = work / "packaged-output"
        execute(extracted / "src", sample / "common-passwords.txt", sample / "hashes.txt", output)
        if output.read_bytes() != (sample / "expected.txt").read_bytes():
            raise RuntimeError("Extracted submission failed the sample")
        destination.parent.mkdir(exist_ok=True)
        # Destination may live on a different filesystem than the temporary folder.
        staging = destination.with_suffix(".zip.tmp")
        staging.write_bytes(package.read_bytes())
        os.replace(staging, destination)
        print(f"PASS extracted submission; created {destination}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, subprocess.TimeoutExpired, zipfile.BadZipFile, KeyError) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        print("No new submission ZIP created; any older dist/project2.zip is stale.", file=sys.stderr)
        sys.exit(1)
