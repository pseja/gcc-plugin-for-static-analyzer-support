#!/usr/bin/env python3

import argparse
import difflib
import glob
import os
import re
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OLD_PLUGIN = os.path.join(
    SCRIPT_DIR, "predator", "cl_build", "tests", "libcl_smoke_test.so"
)
NEW_PLUGIN = os.path.join(SCRIPT_DIR, "build", "libcl_gcc.so")


def normalize_dump(text: str) -> str:
    """Normalize pp-dump text so old and new pipeline outputs are comparable"""

    # prefix every block label with the enclosing function name so same-numbered blocks from different functions don't collide
    out_lines = []
    current_func = ""
    for line in text.splitlines():
        if re.match(r"^[a-zA-Z_]\w*\([^\)]*\):", line):
            current_func = line.split("(")[0]

        def brepl(m):
            return f"{current_func}_{m.group(0)}"

        line = re.sub(r"bb_\d+|L\d+", brepl, line)
        out_lines.append(line)

    text = "\n".join(out_lines)

    # replace prefixed block labels with stable sequential names
    blocks: dict[str, str] = {}

    def block_repl(m):
        b = m.group(0)
        if b not in blocks:
            blocks[b] = f"BLOCK_{len(blocks)}"
        return blocks[b]

    text = re.sub(r"[a-zA-Z_]\w*_(?:bb_\d+|L\d+)", block_repl, text)

    # replace compiler-generated variable names with stable sequential names
    vars_map: dict[str, str] = {}

    def var_repl(m):
        v = m.group(0)
        if v not in vars_map:
            vars_map[v] = f"VAR_{len(vars_map)}"
        return vars_map[v]

    text = re.sub(r"%[a-zA-Z0-9_:\.]+", var_repl, text)

    return text


def resolve(path: str) -> str:
    return path if os.path.isabs(path) else os.path.join(SCRIPT_DIR, path)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Runs CodeListener integration tests")
    parser.add_argument(
        "--test-dir",
        "-t",
        default="tests/integration_tests",
        help="Directory containing .c test files (default: tests/integration_tests)",
    )
    parser.add_argument(
        "--output-dir",
        "-o",
        default="test_output",
        help="Directory for test artifacts (default: test_output)",
    )
    args = parser.parse_args()

    test_dir = resolve(args.test_dir)
    output_dir = resolve(args.output_dir)
    os.makedirs(output_dir, exist_ok=True)

    test_files = sorted(glob.glob(os.path.join(test_dir, "*.c")))
    passed = 0
    failed = 0

    print(f"Running {len(test_files)} tests from {os.path.relpath(test_dir)}/")
    print()

    for test in test_files:
        abs_test = os.path.abspath(test)
        base = os.path.splitext(os.path.basename(test))[0]
        print(f"  {base} ... ", end="", flush=True)

        pp_old = os.path.join(output_dir, f"{base}.pp_old.txt")
        pp_new = os.path.join(output_dir, f"{base}.pp_new.txt")
        diff_path = os.path.join(output_dir, f"{base}.diff")

        # old pipeline
        cmd_old = [
            "gcc-12",
            "-O0",
            "-c",
            abs_test,
            f"-fplugin={OLD_PLUGIN}",
            f"-fplugin-arg-libcl_smoke_test-dump-pp={pp_old}",
            "-o",
            "/dev/null",
        ]
        res_old = subprocess.run(cmd_old, capture_output=True, text=True)
        if res_old.returncode != 0 or not os.path.exists(pp_old):
            print("FAIL [old pipeline crashed]")
            if res_old.stderr.strip():
                print(f"    {res_old.stderr.strip().splitlines()[0]}")
            failed += 1
            continue

        # new pipeline
        raw_new = os.path.join(output_dir, "dump_new.txt")
        if os.path.exists(raw_new):
            os.remove(raw_new)

        cmd_new = [
            "gcc-12",
            "-O0",
            "-c",
            abs_test,
            f"-fplugin={NEW_PLUGIN}",
            "-o",
            "/dev/null",
        ]
        res_new = subprocess.run(
            cmd_new, capture_output=True, text=True, cwd=output_dir
        )
        if res_new.returncode != 0 or not os.path.exists(raw_new):
            print("FAIL [new pipeline crashed]")
            if res_new.stderr.strip():
                print(
                    f"    {'\n    '.join(res_new.stderr.strip().splitlines()[-9:])}")
            failed += 1
            continue

        os.rename(raw_new, pp_new)

        # normalize and compare results
        with open(pp_old) as f:
            old_norm = normalize_dump(f.read())
        with open(pp_new) as f:
            new_norm = normalize_dump(f.read())

        if old_norm == new_norm:
            print("PASS")
            passed += 1
            # remove artifacts left by a previous failure
            for p in (pp_old, pp_new, diff_path):
                if os.path.exists(p):
                    os.remove(p)
        else:
            print("FAIL [output mismatch]")
            failed += 1

            diff_lines = list(
                difflib.unified_diff(
                    old_norm.splitlines(keepends=True),
                    new_norm.splitlines(keepends=True),
                    fromfile=f"{base}.pp_old.txt (normalized)",
                    tofile=f"{base}.pp_new.txt (normalized)",
                )
            )
            with open(diff_path, "w") as f:
                f.writelines(diff_lines)

            preview = diff_lines[:15]
            for line in preview:
                print(f"    {line}", end="")
            if len(diff_lines) > len(preview):
                print(
                    f"\n    ... ({len(diff_lines) - len(preview)} more lines, see {os.path.relpath(diff_path)})"
                )
            else:
                print()

    print()
    print(
        f"Results: {passed} passed, {failed} failed out of {passed + failed} tests")
    if failed:
        print(f"Artifacts written to: {output_dir}")


if __name__ == "__main__":
    main()
