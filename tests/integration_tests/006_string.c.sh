#!/bin/bash

# @file 006_string.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests for char* variable backed by an ARRAY-of-char type
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

# INTEGER type named 'char' (8-bit) must exist
jq -e '.types[] | select(.kind == "INTEGER" and .name == "char")' "$JSON" >/dev/null ||
	fail "INTEGER type 'char' not found"

# POINTER type must exist (char*)
jq -e '.types[] | select(.kind == "POINTER")' "$JSON" >/dev/null ||
	fail "no POINTER type found"

# POINTER type must point to 'char' (or to an ARRAY whose element is char)
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "POINTER") |
          select(
              ($t[.pointee_type_id|tostring].name  == "char") or
              ($t[.pointee_type_id|tostring].kind  == "ARRAY")
          ))' \
	"$JSON" >/dev/null || fail "POINTER should target char or an ARRAY of char"

# ARRAY type of char exists (the string literal storage)
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "ARRAY") |
          select($t[.element_type_id|tostring].name == "char"))' \
	"$JSON" >/dev/null || fail "no ARRAY-of-char type found for string literal"

# variable 'str' is present
jq -e '.variables[] | select(.name == "str")' "$JSON" >/dev/null ||
	fail "variable 'str' not found"

# at least one ASSIGN instruction
N=$(jq '[.instructions[] | select(.kind == "ASSIGN")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN instruction"
