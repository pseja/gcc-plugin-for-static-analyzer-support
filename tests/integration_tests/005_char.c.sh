#!/bin/bash

# @file 005_char.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that char is an 8-bit INTEGER type
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

# INTEGER type named 'char' exists with size 8
jq -e '.types[] | select(.kind == "INTEGER" and .name == "char")' "$JSON" >/dev/null ||
	fail "INTEGER type 'char' not found"

SIZE=$(jq 'first(.types[] | select(.name == "char")) | .size_bits' "$JSON")
[ "$SIZE" -eq 8 ] || fail "char should be 8 bits, got $SIZE"

# variable 'c' has type 'char'
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.variables[] | select(.name == "c") |
          select($t[.type_id|tostring].name == "char"))' \
	"$JSON" >/dev/null || fail "variable 'c' should have type 'char'"

# at least one ASSIGN (the char initialisation)
N=$(jq '[.instructions[] | select(.kind == "ASSIGN")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN instruction"
