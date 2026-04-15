#!/bin/bash

# @file 013_while.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests while loop, specifically COND instruction, back-edge (block with itself as predecessor)
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

# COND instruction must exist (the while test)
jq -e '.instructions[] | select(.kind == "COND")' "$JSON" >/dev/null ||
	fail "no COND instruction found (expected while-loop test)"

# the COND must have both true and false targets
jq -e '
    first(.instructions[] | select(.kind == "COND") |
          select(.true_target != null and .false_target != null))' \
	"$JSON" >/dev/null || fail "COND should have true_target and false_target"

# at least one block has >= 2 predecessors (the loop header)
N=$(jq '[.blocks[] | select(.predecessors | length >= 2)] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected a loop-header block with >= 2 predecessors"

# ASSIGN must exist (the x /= 2 body)
N=$(jq '[.instructions[] | select(.kind == "ASSIGN")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN instruction"
