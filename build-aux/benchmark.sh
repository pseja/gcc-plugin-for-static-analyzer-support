#!/usr/bin/env bash
# @file benchmark.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Compare old Code Listener (libsl.so) vs new (libcl_gcc.so + 
# libsl_analyzer.so) on the Predator regression test suite.
#
# @date 2026-05-10
#
# @copyright Copyright (c) 2026 Lukáš Pšeja
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

OLD_PLUGIN="$ROOT/build/analyzers/predator/sl_build/libsl.so"
NEW_PLUGIN="$ROOT/build/libcl_gcc.so"
NEW_ANALYZER="$ROOT/build/analyzers/predator/sl_build/libsl_analyzer.so"
INCDIR="$ROOT/analyzers/predator/include/predator-builtins"
TEST_DIR="$ROOT/analyzers/predator/tests/predator-regre"
MAKE_CONFIG="$ROOT/build/make-config.mk"

ITERATIONS=1
FILTER="*.c"
TIMEOUT=30
CSV_FILE=""
DRY_RUN=0
VERBOSE=0
RUN_OLD=1
RUN_NEW=1

die() {
    printf >&2 'benchmark: error: %s\n' "$*"
    exit 1
}

usage() {
    cat <<'EOF'
Usage: benchmark.sh [OPTIONS]

Compare old Code Listener (libsl.so) vs new (libcl_gcc.so + libsl_analyzer.so)
on the Predator regression test suite.

OPTIONS
  -i, --iterations N    Repeat each file N times (default: 1)
  -f, --filter GLOB     Only files matching GLOB (default: *.c)
  -t, --timeout SEC     Per-file timeout in seconds (default: 30)
      --csv FILE        Write per-file timings and counters to FILE
      --dry-run         Skip Predator analysis; measure front-end overhead only
      --old-only        Only run the OLD suite
      --new-only        Only run the NEW suite
  -v, --verbose         Print each filename as it is processed
  -h, --help            Print this help and exit
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
    -i | --iterations)
        ITERATIONS="$2"; shift 2
        [[ "$ITERATIONS" =~ ^[1-9][0-9]*$ ]] || die "--iterations must be a positive integer"
        ;;
    -f | --filter)
        FILTER="$2"; shift 2
        ;;
    -t | --timeout)
        TIMEOUT="$2"; shift 2
        [[ "$TIMEOUT" =~ ^[1-9][0-9]*$ ]] || die "--timeout must be a positive integer"
        ;;
    --csv)
        CSV_FILE="$2"; shift 2
        ;;
    --dry-run)
        DRY_RUN=1; shift
        ;;
    --old-only)
        RUN_OLD=1; RUN_NEW=0; shift
        ;;
    --new-only)
        RUN_OLD=0; RUN_NEW=1; shift
        ;;
    -v | --verbose)
        VERBOSE=1; shift
        ;;
    -h | --help)
        usage; exit 0
        ;;
    *)
        printf >&2 'benchmark: error: Unknown argument: '\''%s'\''\n\n' "$1"
        usage >&2
        exit 1
        ;;
    esac
done

[[ -f "$MAKE_CONFIG" ]] ||
    die "Build not configured: $MAKE_CONFIG not found.\nRun 'make configure' first."

GCC=$(grep '^TARGET_GCC :=' "$MAKE_CONFIG" | awk '{print $3}')
[[ -n "$GCC" && -x "$GCC" ]] ||
    die "Cannot read TARGET_GCC from $MAKE_CONFIG or binary not executable."

check_plugin() {
    local path="$1" hint="$2"
    if [[ ! -f "$path" ]]; then
        printf >&2 'benchmark: error: plugin not found: %s\n' "$path"
        printf >&2 '  Hint: %s\n' "$hint"
        exit 1
    fi
}

[[ $RUN_OLD -eq 1 ]] && check_plugin "$OLD_PLUGIN" \
    "cmake --build build/analyzers/predator/sl_build --target sl -j\$(nproc)"
if [[ $RUN_NEW -eq 1 ]]; then
    check_plugin "$NEW_PLUGIN" "make build"
    check_plugin "$NEW_ANALYZER" \
        "cmake --build build/analyzers/predator/sl_build --target sl_analyzer -j\$(nproc)"
fi

[[ -d "$INCDIR" ]] || die "Predator include directory not found: $INCDIR"
[[ -d "$TEST_DIR" ]] || die "Test directory not found: $TEST_DIR"

mapfile -t TEST_FILES < <(find "$TEST_DIR" -name "$FILTER" | sort)
N_FILES=${#TEST_FILES[@]}
[[ $N_FILES -gt 0 ]] || die "No test files found in $TEST_DIR matching '$FILTER'"

ns_now() {
    local us="${EPOCHREALTIME/./}"
    echo $((us * 1000))
}

fmt_ns() {
    local ns=$1
    local ms=$((ns / 1000000))
    if [[ $ms -ge 60000 ]]; then
        printf '%dm%d.%03ds' "$((ms / 60000))" "$(((ms % 60000) / 1000))" "$((ms % 1000))"
    elif [[ $ms -ge 1000 ]]; then
        printf '%d.%03ds' "$((ms / 1000))" "$((ms % 1000))"
    else
        printf '%dms' "$ms"
    fi
}

pct_change() {
    local old_ns=$1 new_ns=$2
    if [[ $old_ns -eq 0 ]]; then echo "N/A"; return; fi
    local diff=$((new_ns - old_ns))
    local pct=$((diff * 100 / old_ns))
    if [[ $pct -ge 0 ]]; then printf '+%d' "$pct"; else printf '%d' "$pct"; fi
}

print_table() {
    local title="$1"
    local header_aligns="$2"
    local row_aligns="$3"
    awk -v title="$title" -v haligns="$header_aligns" -v raligns="$row_aligns" '
        BEGIN {
            FS = "\t"
            n_cols = split(haligns, halign_arr, " ")
            split(raligns, ralign_arr, " ")
            num_rows = 0
        }
        {
            num_rows++
            for (i = 1; i <= n_cols; i++) {
                val = $i
                rows[num_rows, i] = val
                if (length(val) > widths[i]) {
                    widths[i] = length(val)
                }
            }
        }
        function rep(char, n,    i, out) {
            out = ""
            for (i = 1; i <= n; i++) out = out char
            return out
        }
        function make_border(    i, out) {
            out = "+"
            for (i = 1; i <= n_cols; i++) {
                out = out rep("-", widths[i] + 2) "+"
            }
            return out
        }
        function make_title_border(    inner_width, i) {
            inner_width = 0
            for (i = 1; i <= n_cols; i++) inner_width += widths[i]
            inner_width += 3 * n_cols - 1
            return "+" rep("-", inner_width) "+"
        }
        function make_title_row(    inner_width, total_pad, left_pad, right_pad, i) {
            inner_width = 0
            for (i = 1; i <= n_cols; i++) inner_width += widths[i]
            inner_width += 3 * n_cols - 1
            total_pad = inner_width > length(title) ? inner_width - length(title) : 0
            left_pad = int(total_pad / 2)
            right_pad = total_pad - left_pad
            return "|" rep(" ", left_pad) title rep(" ", right_pad) "|"
        }
        function align_cell(val, w, is_left,    pad, padding) {
            pad = w - length(val)
            if (pad < 0) pad = 0
            padding = rep(" ", pad)
            if (is_left) return val padding
            return padding val
        }
        function make_row(r, is_header,    i, out, is_left) {
            out = "|"
            for (i = 1; i <= n_cols; i++) {
                if (is_header) is_left = (halign_arr[i] == "L")
                else is_left = (ralign_arr[i] == "L")
                out = out " " align_cell(rows[r, i], widths[i], is_left) " |"
            }
            return out
        }
        END {
            if (num_rows <= 1) exit

            border = make_border()
            print make_title_border()
            print make_title_row()
            print border
            print make_row(1, 1)
            print border
            for (r = 2; r <= num_rows; r++) {
                print make_row(r, 0)
            }
            print border
        }
    '
}

[[ -n "$CSV_FILE" ]] && printf 'suite,file,iteration,elapsed_ns,model_functions,model_blocks,model_instructions,model_variables,model_types\n' >"$CSV_FILE"

declare -i BENCH_TOTAL_NS=0
declare -i BENCH_FAILED=0
declare -A BENCH_STAGE_TOTAL_NS=()
declare -A BENCH_STAGE_TOTAL_COUNT=()
declare -a BENCH_STAGE_ORDER=()
declare -A BENCH_COUNTER_TOTAL=()
declare -A BENCH_COUNTER_SAMPLE_COUNT=()
declare -a BENCH_COUNTER_ORDER=()

bench_suite() {
    local label="$1"
    local -n _args="$2"
    local capture_statistics="${3:-0}"

    BENCH_TOTAL_NS=0
    BENCH_FAILED=0
    BENCH_STAGE_TOTAL_NS=()
    BENCH_STAGE_TOTAL_COUNT=()
    BENCH_STAGE_ORDER=()
    BENCH_COUNTER_TOTAL=()
    BENCH_COUNTER_SAMPLE_COUNT=()
    BENCH_COUNTER_ORDER=()

    printf >&2 '\nRunning %s suite... ' "$label"
    local total_runs=$((N_FILES * ITERATIONS))
    local run_no=0
    local tmpdir
    tmpdir=$(mktemp -d)
    local stderr_tmp=""
    [[ $capture_statistics -eq 1 ]] && stderr_tmp=$(mktemp)
    trap "rm -rf '$tmpdir' ${stderr_tmp:+\"$stderr_tmp\"}" RETURN

    local iter file t0 t1 elapsed_ns exit_code
    local -A run_counter_total=()
    local -A run_counter_sample_count=()
    for ((iter = 1; iter <= ITERATIONS; iter++)); do
        for file in "${TEST_FILES[@]}"; do
            ((run_no++)) || true
            printf >&2 '\rRunning %s suite... %d/%d' "$label" "$run_no" "$total_runs"

            t0=$(ns_now)
            exit_code=0
            run_counter_total=()
            run_counter_sample_count=()

            if [[ $capture_statistics -eq 1 ]]; then
                (cd "$tmpdir"; timeout "$TIMEOUT" "$GCC" "${_args[@]}" "$file") 2>"$stderr_tmp" || exit_code=$?

                while IFS=$'\t' read -r kind name cnt total; do
                    case "$kind" in
                    timing)
                        if [[ ! "${BENCH_STAGE_TOTAL_NS[$name]+_}" ]]; then
                            BENCH_STAGE_ORDER+=("$name")
                            BENCH_STAGE_TOTAL_NS["$name"]=0
                            BENCH_STAGE_TOTAL_COUNT["$name"]=0
                        fi
                        BENCH_STAGE_TOTAL_NS["$name"]=$((BENCH_STAGE_TOTAL_NS["$name"] + total))
                        BENCH_STAGE_TOTAL_COUNT["$name"]=$((BENCH_STAGE_TOTAL_COUNT["$name"] + cnt))
                        ;;
                    counter)
                        if [[ ! "${BENCH_COUNTER_TOTAL[$name]+_}" ]]; then
                            BENCH_COUNTER_ORDER+=("$name")
                            BENCH_COUNTER_TOTAL["$name"]=0
                            BENCH_COUNTER_SAMPLE_COUNT["$name"]=0
                        fi
                        BENCH_COUNTER_TOTAL["$name"]=$((BENCH_COUNTER_TOTAL["$name"] + total))
                        BENCH_COUNTER_SAMPLE_COUNT["$name"]=$((BENCH_COUNTER_SAMPLE_COUNT["$name"] + cnt))
                        run_counter_total["$name"]=$(( ${run_counter_total["$name"]:-0} + total ))
                        run_counter_sample_count["$name"]=$(( ${run_counter_sample_count["$name"]:-0} + cnt ))
                        ;;
                    esac
                done < <(awk '
                    function trim(s) { gsub(/^[ \t]+|[ \t]+$/, "", s); return s }
                    BEGIN { section = "" }
                    { gsub(/\033\[[0-9;]*m/, "") }
                    /note: / {
                        msg = $0
                        sub(/.*note: /, "", msg)
                        if (msg ~ /^\+/) { next }
                        if (msg ~ /^\|/) {
                            inner = msg
                            sub(/^\|[ ]?/, "", inner); sub(/[ ]?\|$/, "", inner)
                            n = split(inner, raw_cells, /\|/)
                            delete cells
                            for (i = 1; i <= n; i++) { cells[i] = trim(raw_cells[i]) }
                            if (n == 1 && cells[1] == "Timing Summary")  { section = "timing";  next }
                            if (n == 1 && cells[1] == "Counter Summary") { section = "counter"; next }
                            if (n == 4 && (cells[1] == "Stage Name" || cells[1] == "Counter Name")) { next }
                            if (n == 4 && cells[2] ~ /^[0-9]+$/) {
                                if (section == "timing" && cells[3] ~ /^[0-9]+\.[0-9]+$/) {
                                    print "timing\t" cells[1] "\t" cells[2] "\t" int(cells[3] * 1000000 + 0.5)
                                } else if (section == "counter" && cells[3] ~ /^[0-9]+$/) {
                                    print "counter\t" cells[1] "\t" cells[2] "\t" cells[3]
                                }
                            }
                            next
                        }
                        if (msg == "Timing summary:")  { section = "timing";  next }
                        if (msg == "Counter summary:") { section = "counter"; next }
                        if (length(msg) < 52 || msg !~ /^[a-z]/ || msg !~ /[0-9]\.[0-9][0-9][0-9]$/) { next }
                        name = substr(msg, 1, 40); gsub(/[ \t]+$/, "", name)
                        rest = substr(msg, 41); n = split(rest, f)
                        if (n != 3 || f[1] !~ /^[0-9]+$/) { next }
                        if (section == "timing" && f[2] ~ /^[0-9]+\.[0-9]+$/) {
                            print "timing\t" name "\t" f[1] "\t" int(f[2] * 1000000 + 0.5)
                        } else if (section == "counter" && f[2] ~ /^[0-9]+$/) {
                            print "counter\t" name "\t" f[1] "\t" f[2]
                        }
                    }
                ' "$stderr_tmp")
            else
                (cd "$tmpdir"; timeout "$TIMEOUT" "$GCC" "${_args[@]}" "$file") 2>/dev/null || exit_code=$?
            fi

            t1=$(ns_now)
            elapsed_ns=$((t1 - t0))
            BENCH_TOTAL_NS=$((BENCH_TOTAL_NS + elapsed_ns))
            [[ $exit_code -ne 0 ]] && ((BENCH_FAILED++)) || true
            [[ $VERBOSE -eq 1 ]] && printf >&2 '\n  [%s] %s %s\n' "$label" "$(fmt_ns "$elapsed_ns")" "$file"

            if [[ -n "$CSV_FILE" ]]; then
                printf '%s,%s,%d,%d,%s,%s,%s,%s,%s\n' \
                    "$label" "$(basename "$file")" "$iter" "$elapsed_ns" \
                    "${run_counter_total[model:functions]:-}" \
                    "${run_counter_total[model:blocks]:-}" \
                    "${run_counter_total[model:instructions]:-}" \
                    "${run_counter_total[model:variables]:-}" \
                    "${run_counter_total[model:types]:-}" \
                    >>"$CSV_FILE"
            fi
        done
    done
    printf >&2 '\rRunning %s suite... done.\033[K\n' "$label"
}

OLD_ARGS=(
    -std=gnu99 -m64 -S -o /dev/null
    -I"$INCDIR" -DPREDATOR
    -fplugin="$OLD_PLUGIN"
    -fplugin-arg-libsl-args=error_label:ERROR
    -fplugin-arg-libsl-preserve-ec
)
[[ $DRY_RUN -eq 1 ]] && OLD_ARGS+=(-fplugin-arg-libsl-dry-run)

NEW_ARGS=(
    -std=gnu99 -m64 -S -o /dev/null
    -I"$INCDIR" -DPREDATOR
    -fplugin="$NEW_PLUGIN"
    "-fplugin-arg-libcl_gcc-load-analyzer=$NEW_ANALYZER"
    -fplugin-arg-libcl_gcc-args=error_label:ERROR
    -fplugin-arg-libcl_gcc-statistics
)
[[ $DRY_RUN -eq 1 ]] && NEW_ARGS+=(-fplugin-arg-libcl_gcc-dry-run)

printf '\n'
printf '================================================================================\n'
printf '                             Code Listener Benchmark\n'
printf '================================================================================\n'
printf '  GCC        : %s\n' "$GCC"
printf '  Test dir   : %s\n' "$TEST_DIR"
printf '  Files      : %d (filter: %s)\n' "$N_FILES" "$FILTER"
printf '  Iterations : %d (%d runs per iteration)\n' "$ITERATIONS" "$((N_FILES * ITERATIONS))"
printf '  Timeout    : %ds per file\n' "$TIMEOUT"
printf '  Mode       : %s\n' "$([ $DRY_RUN -eq 1 ] && echo 'CL front-end only' || echo 'full Predator analysis')"
[[ -n "$CSV_FILE" ]] && printf '  CSV output : %s\n' "$CSV_FILE"
printf -- '--------------------------------------------------------------------------------\n'
printf '  OLD : %s\n' "$OLD_PLUGIN"
printf '  NEW : %s\n' "$NEW_PLUGIN"
printf '      + %s\n' "$NEW_ANALYZER"
printf '================================================================================\n'

OLD_TOTAL_NS=0; OLD_FAILED=0
NEW_TOTAL_NS=0; NEW_FAILED=0

if [[ $RUN_OLD -eq 1 ]]; then
    bench_suite "OLD" OLD_ARGS
    OLD_TOTAL_NS=$BENCH_TOTAL_NS
    OLD_FAILED=$BENCH_FAILED
fi

if [[ $RUN_NEW -eq 1 ]]; then
    bench_suite "NEW" NEW_ARGS 1
    NEW_TOTAL_NS=$BENCH_TOTAL_NS
    NEW_FAILED=$BENCH_FAILED
fi

printf '\n'

TOTAL_RUNS=$((N_FILES * ITERATIONS))

# results table
{
    printf 'Suite\tTotal Time\tMean / File\tFailures\n'
    if [[ $RUN_OLD -eq 1 ]]; then
        OLD_MEAN_NS=$((OLD_TOTAL_NS / TOTAL_RUNS))
        printf '%s\t%s\t%s\t%d\n' 'OLD' "$(fmt_ns "$OLD_TOTAL_NS")" "$(fmt_ns "$OLD_MEAN_NS")" "$OLD_FAILED"
    fi

    if [[ $RUN_NEW -eq 1 ]]; then
        NEW_MEAN_NS=$((NEW_TOTAL_NS / TOTAL_RUNS))
        printf '%s\t%s\t%s\t%d\n' 'NEW' "$(fmt_ns "$NEW_TOTAL_NS")" "$(fmt_ns "$NEW_MEAN_NS")" "$NEW_FAILED"
    fi
} | print_table "Results" "L L L L" "L R R R"

if [[ $RUN_OLD -eq 1 && $RUN_NEW -eq 1 && $OLD_TOTAL_NS -gt 0 ]]; then
    PCT=$(pct_change "$OLD_TOTAL_NS" "$NEW_TOTAL_NS")
    if [[ $NEW_TOTAL_NS -lt $OLD_TOTAL_NS ]]; then
        VERDICT="NEW is faster (${PCT}%)"
    elif [[ $NEW_TOTAL_NS -gt $OLD_TOTAL_NS ]]; then
        VERDICT="NEW is slower (${PCT}%)"
    else
        VERDICT="No difference"
    fi
    printf 'Result: %s\n' "$VERDICT"
fi
printf '\n'

# timing summary table
if [[ $RUN_NEW -eq 1 && ${#BENCH_STAGE_ORDER[@]} -gt 0 ]]; then
    {
        printf 'Stage Name\tCalls\tTotal (ms)\tMean (ms)\n'
        for stage in "${BENCH_STAGE_ORDER[@]}"; do
            _ns=${BENCH_STAGE_TOTAL_NS[$stage]}
            _cnt=${BENCH_STAGE_TOTAL_COUNT[$stage]}
            awk -v name="$stage" -v ns="$_ns" -v cnt="$_cnt" -v runs="$TOTAL_RUNS" \
                'BEGIN { printf "%s\t%d\t%.3f\t%.3f\n", name, cnt, ns/1000000, ns/runs/1000000 }'
        done
    } | print_table "Timing Summary" "L L L L" "L R R R"
    printf '\n'
fi

# counter summary table
if [[ $RUN_NEW -eq 1 && ${#BENCH_COUNTER_ORDER[@]} -gt 0 ]]; then
    {
        printf 'Counter Name\tSamples\tTotal\tMean\n'
        for counter in "${BENCH_COUNTER_ORDER[@]}"; do
            _total=${BENCH_COUNTER_TOTAL[$counter]}
            _samples=${BENCH_COUNTER_SAMPLE_COUNT[$counter]}
            awk -v name="$counter" -v total="$_total" -v samples="$_samples" -v runs="$TOTAL_RUNS" \
                'BEGIN { printf "%s\t%d\t%d\t%.3f\n", name, samples, total, total/runs }'
        done
    } | print_table "Counter Summary" "L L L L" "L R R R"
    printf '\n'
fi

# model build cost
if [[ $RUN_NEW -eq 1 && ${BENCH_STAGE_TOTAL_NS[gimple_to_codemodel]+_} && ${#BENCH_COUNTER_ORDER[@]} -gt 0 ]]; then
    GIMPLE_TOTAL_NS=${BENCH_STAGE_TOTAL_NS[gimple_to_codemodel]}
    {
        printf 'Counter Name\tTotal\tMean\tLowering (us/item)\n'
        for counter in model:functions model:blocks model:instructions model:variables model:types; do
            [[ ! "${BENCH_COUNTER_TOTAL[$counter]+_}" ]] && continue
            _total=${BENCH_COUNTER_TOTAL[$counter]}
            [[ $_total -le 0 ]] && continue
            awk -v name="$counter" -v total="$_total" -v runs="$TOTAL_RUNS" -v gimple_ns="$GIMPLE_TOTAL_NS" \
                'BEGIN { printf "%s\t%d\t%.3f\t%.3f\n", name, total, total/runs, gimple_ns/total/1000.0 }'
        done
    } | print_table "Model build cost" "L L L L" "L R R R"
    printf '\n'
fi

[[ -n "$CSV_FILE" ]] && printf 'Output written to: %s\n\n' "$CSV_FILE" || true
