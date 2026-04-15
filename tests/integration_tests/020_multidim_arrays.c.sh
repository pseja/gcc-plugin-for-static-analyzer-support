#!/bin/bash

# @file 020_multidim_arrays.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests nested ARRAY types (ARRAY of ARRAY of int)
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

# two distinct ARRAY types must exist (one is array-of-array)
N=$(jq '[.types[] | select(.kind == "ARRAY")] | length' "$JSON")
[ "$N" -ge 2 ] || fail "expected at least 2 ARRAY types for int[3][3], got $N"

# the outer array has element_count == 3
OUTER=$(jq '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "ARRAY") |
          select($t[.element_type_id|tostring].kind == "ARRAY")) |
    .element_count' "$JSON")
[ "$OUTER" -eq 3 ] || fail "outer array element_count should be 3, got $OUTER"

# the inner array also has element_count == 3
INNER=$(jq '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "ARRAY") |
          select($t[.element_type_id|tostring].kind == "INTEGER")) |
    .element_count' "$JSON")
[ "$INNER" -eq 3 ] || fail "inner array element_count should be 3, got $INNER"

# at least one ASSIGN with an ARRAY accessor (matrix element access)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(
         ([(.lhs.access_path  // [])[] | select(.kind == "ARRAY")] | length > 0) or
         ([(.rhs1.access_path // [])[] | select(.kind == "ARRAY")] | length > 0)
     )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN with ARRAY accessor"
