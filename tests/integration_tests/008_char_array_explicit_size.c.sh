#!/bin/bash

# @file 008_char_array_explicit_size.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests ARRAY-of-char with element_count == 42
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

# ARRAY type of char must exist
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "ARRAY") |
          select($t[.element_type_id|tostring].name == "char"))' \
	"$JSON" >/dev/null || fail "no ARRAY-of-char type found"

# element_count should be 42 (explicitly declared size)
N=$(jq '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "ARRAY") |
          select($t[.element_type_id|tostring].name == "char")) |
    .element_count' "$JSON")
[ "$N" -eq 42 ] || fail "char array element_count should be 42 (explicit), got $N"

# variable 'arr' exists with ARRAY type
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.variables[] | select(.name == "arr") |
          select($t[.type_id|tostring].kind == "ARRAY"))' \
	"$JSON" >/dev/null || fail "variable 'arr' not found with ARRAY type"
