#!/bin/bash

# @file 024_complex_initializers.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief ARRAY of STRUCT vec2i, ARRAY+FIELD access paths for aggregate element access
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

# STRUCT type 'vec2i' exists
jq -e '.types[] | select(.kind == "STRUCT" and .name == "vec2i")' "$JSON" >/dev/null ||
	fail "STRUCT type 'vec2i' not found"

# ARRAY type whose element is the vec2i struct exists
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "ARRAY") |
          select($t[.element_type_id|tostring].name == "vec2i"))' \
	"$JSON" >/dev/null || fail "no ARRAY-of-vec2i type found"

# ASSIGN instructions with combined ARRAY+FIELD access path (initialization of arr[i].members)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(
         ([(.lhs.access_path  // [])[] | select(.kind == "ARRAY")] | length > 0) or
         ([(.rhs1.access_path // [])[] | select(.kind == "ARRAY")] | length > 0)
     ) |
     select(
         ([(.lhs.access_path  // [])[] | select(.kind == "FIELD")] | length > 0) or
         ([(.rhs1.access_path // [])[] | select(.kind == "FIELD")] | length > 0)
     )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected ASSIGN with combined ARRAY+FIELD access path (arr[i].member)"
