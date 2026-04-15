#!/bin/bash

# @file 004_double_to_int_cast.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests REAL type for double (64-bit), cast to int
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

# REAL type named 'double' exists and is 64-bit
jq -e '.types[] | select(.kind == "REAL" and .name == "double")' "$JSON" >/dev/null ||
	fail "REAL type 'double' not found"

SIZE=$(jq 'first(.types[] | select(.kind == "REAL" and .name == "double")) | .size_bits' "$JSON")
[ "$SIZE" -eq 64 ] || fail "double should be 64 bits, got $SIZE"

# variable 'd' has REAL type
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.variables[] | select(.name == "d") |
          select($t[.type_id|tostring].kind == "REAL"))' \
	"$JSON" >/dev/null || fail "variable 'd' should have REAL type"

# INTEGER type named 'int' exists
jq -e '.types[] | select(.kind == "INTEGER" and .name == "int")' "$JSON" >/dev/null ||
	fail "INTEGER type 'int' not found"

# at least one ASSIGN instruction (cast lowers to ASSIGN in GIMPLE)
N=$(jq '[.instructions[] | select(.kind == "ASSIGN")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN instruction"
