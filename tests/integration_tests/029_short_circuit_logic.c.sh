#!/bin/bash

# @file 029_short_circuit_logic.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that && and || lower to multiple COND blocks
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

# function 'check' and 'main' exist
jq -e '.functions[] | select(.name == "check")' "$JSON" >/dev/null ||
	fail "function 'check' not found"
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# main() should have >= 2 COND instructions for short-circuit evaluation
MAIN_ID=$(jq '.functions[] | select(.name == "main") | .id' "$JSON")
N=$(jq --argjson fid "$MAIN_ID" '
    [.instructions[] | select(.kind == "COND") |
     .parent_block_id as $bid |
     .blocks[] | select(.id == $bid and .parent == $fid)] |
    length' "$JSON" 2>/dev/null ||
	jq '[.instructions[] | select(.kind == "COND")] | length' "$JSON")
[ "$N" -ge 2 ] || fail "expected >= 2 COND instructions for short-circuit evaluation, got $N"

# at least one block with >= 2 predecessors (the merge point after short-circuit)
MAIN_BLOCK_IDS=$(jq --argjson fid "$MAIN_ID" '
    [.blocks[] | select(.parent == $fid) | .id]' "$JSON")
N=$(jq --argjson ids "$MAIN_BLOCK_IDS" '
    [.blocks[] |
     select(.id as $id | $ids | index($id) != null) |
     select(.name != "ENTRY" and .name != "EXIT") |
     select(.predecessors | length >= 2)] |
    length' "$JSON")
[ "$N" -ge 1 ] || fail "expected a merge-point block with >= 2 predecessors in main()"

# CALL instruction (check() calls)
N=$(jq '[.instructions[] | select(.kind == "CALL")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected a CALL instruction (check(b) call)"
