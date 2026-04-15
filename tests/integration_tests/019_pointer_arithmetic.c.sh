#!/bin/bash

# @file 019_pointer_arithmetic.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests ARRAY type with element_count == 5, pointer arithmetic
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

# ARRAY type must exist
jq -e '.types[] | select(.kind == "ARRAY")' "$JSON" >/dev/null ||
	fail "expected at least one ARRAY type"

# array has element_count == 5
N=$(jq 'first(.types[] | select(.kind == "ARRAY")) | .element_count' "$JSON")
[ "$N" -eq 5 ] || fail "array element_count should be 5, got $N"

# POINTER-to-INTEGER type exists (int*)
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "POINTER") |
          select($t[.pointee_type_id|tostring].kind == "INTEGER"))' \
	"$JSON" >/dev/null || fail "expected a POINTER-to-INTEGER type for pointer arithmetic"

# array indexing is lowered to ASSIGN instructions, verify at least one such instructions exists
N=$(jq '[ .instructions[] | select(.kind == "ASSIGN") |
           select(
               ([(.lhs.access_path  // [])[] | select(.kind == "ARRAY")] | length > 0) or
               ([(.rhs1.access_path // [])[] | select(.kind == "ARRAY")] | length > 0)
           )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN with ARRAY accessor (array indexing)"
