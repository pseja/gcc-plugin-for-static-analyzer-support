#!/bin/bash

# @file 028_noreturn_abort.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests ABORT from __builtin_trap and UNREACHABLE from __builtin_unreachable
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

# at least one ABORT instruction (from __builtin_trap)
N=$(jq '[.instructions[] | select(.kind == "ABORT")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ABORT instruction (from __builtin_trap)"

# at least one UNREACHABLE instruction (from __builtin_unreachable)
N=$(jq '[.instructions[] | select(.kind == "UNREACHABLE")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one UNREACHABLE instruction (from __builtin_unreachable)"

# ABORT block must also contain a CALL
ABORT_BID=$(jq 'first(.instructions[] | select(.kind == "ABORT")) | .parent_block_id' "$JSON")
N=$(jq --argjson bid "$ABORT_BID" \
	'[.instructions[] | select(.kind == "CALL" and .parent_block_id == $bid)] | length' "$JSON")
[ "$N" -ge 1 ] || fail "the ABORT block should also contain a CALL to the noreturn function"
