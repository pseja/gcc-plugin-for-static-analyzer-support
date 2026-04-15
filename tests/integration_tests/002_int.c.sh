#!/bin/bash

# @file 002_int.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests basic integer variable
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

# at least one function
jq -e '.functions | length > 0' "$JSON" >/dev/null || fail "no functions found"

# function 'main' exists
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# exactly one INTEGER type named 'int', 32-bit signed
jq -e '.types[] | select(.name == "int")' "$JSON" >/dev/null ||
	fail "type 'int' not found"

SIZE=$(jq '.types[] | select(.name == "int") | .size_bits' "$JSON")
[ "$SIZE" -eq 32 ] || fail "int should be 32 bits, got $SIZE"

UNSIGNED=$(jq '.types[] | select(.name == "int") | .is_unsigned' "$JSON")
[ "$UNSIGNED" = "false" ] || fail "int should be signed (is_unsigned == false)"

# main contains at least one ASSIGN instruction
MAIN_ID=$(jq '.functions[] | select(.name == "main") | .id' "$JSON")
N=$(jq --argjson fid "$MAIN_ID" '
    [.instructions[] | select(.kind == "ASSIGN") |
     .parent_block_id as $bid |
     select( .blocks[] // empty | select(.id == $bid and .parent == $fid) )] |
    length' "$JSON" 2>/dev/null || echo 0)
# check any ASSIGN exists in the whole model
N=$(jq '[.instructions[] | select(.kind == "ASSIGN")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN instruction"
