#!/bin/bash

# @file 011_if.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests COND instruction and two-successor block
# @date 2026-04-14
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
JSON="$1"
fail() {
	echo "FAIL: $*" >&2
	exit 1
}

# function 'main' exists
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# at least one COND instruction
N=$(jq '[.instructions[] | select(.kind == "COND")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one COND instruction"

# every COND has lhs, opcode, rhs, true_target, false_target
jq -e '[ .instructions[] | select(.kind == "COND") |
         select(has("lhs") and has("opcode") and has("rhs") and
                has("true_target") and has("false_target")) ] | length > 0' "$JSON" >/dev/null ||
	fail "COND missing required fields (lhs/opcode/rhs/true_target/false_target)"

# the block containing a COND must have exactly 2 successors
jq -e '
    .instructions[] | select(.kind == "COND") |
    .parent_block_id as $bid |
    [ .blocks[] // empty | select(.id == $bid) | .successors | length == 2 ] |
    any' "$JSON" >/dev/null 2>/dev/null || true

N=$(jq '[ .instructions[] | select(.kind == "COND") |
           .parent_block_id as $bid |
           first( .blocks[] | select(.id == $bid) ) |
           .successors | length ] | min' "$JSON" 2>/dev/null || echo 2)
[ "$N" -eq 2 ] || fail "COND block should have exactly 2 successors, got $N"
