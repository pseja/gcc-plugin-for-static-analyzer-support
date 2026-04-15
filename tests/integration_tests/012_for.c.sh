#!/bin/bash

# @file 012_for.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests loop, specifically that COND exists and a block has >= 2 predecessors
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

# at least one COND (loop condition)
N=$(jq '[.instructions[] | select(.kind == "COND")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one COND instruction (loop condition)"

# at least one block with >= 2 predecessors (the loop header)
N=$(jq '[.blocks[] | select((.predecessors | length) >= 2)] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one block with 2+ predecessors (loop back-edge)"
