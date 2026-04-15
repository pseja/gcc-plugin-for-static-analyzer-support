#!/bin/bash

# @file 031_enums.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests ENUM type 'State', reasonable size, used in a comparison
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

# ENUM type named 'State' must exist
jq -e '.types[] | select(.kind == "ENUM" and .name == "State")' "$JSON" >/dev/null ||
	fail "enum 'State' not found in types"

# Enum size should be one of 8/16/32/64 bits
SIZE=$(jq '.types[] | select(.kind == "ENUM" and .name == "State") | .size_bits' "$JSON")
echo "$SIZE" | grep -qE '^(8|16|32|64)$' || fail "unexpected enum size_bits: $SIZE"

# at least one COND instruction (enum used in a comparison)
N=$(jq '[.instructions[] | select(.kind == "COND")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one COND instruction (enum comparison)"

# enum constant 5 (STATE_RUNNING) or 10 (STATE_ERROR) appears in an ASSIGN
HIT=$(jq '[.instructions[] | select(.kind == "ASSIGN") |
           (.rhs1.value // "") | select(. == "5" or . == "10")] | length' "$JSON")
[ "$HIT" -ge 1 ] || fail "expected enum constant 5 or 10 in an ASSIGN operand"
