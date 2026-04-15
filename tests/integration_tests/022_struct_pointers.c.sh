#!/bin/bash

# @file 022_struct_pointers.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests STRUCT Node with self-referential pointer field, FIELD accessor for member access, and DEREF+FIELD accessor for arrow operator
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

# Checks for 022_struct_pointers.c - .
set -euo pipefail
JSON="$1"
fail() {
	echo "FAIL: $*" >&2
	exit 1
}

# function 'main' exists
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# STRUCT type 'Node' exists
jq -e '.types[] | select(.kind == "STRUCT" and .name == "Node")' "$JSON" >/dev/null ||
	fail "STRUCT type 'Node' not found"

# POINTER type exists (for struct Node*)
jq -e '.types[] | select(.kind == "POINTER")' "$JSON" >/dev/null ||
	fail "no POINTER type found"

# at least one ASSIGN with a FIELD accessor (member access via dot)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(
         ([(.lhs.access_path  // [])[] | select(.kind == "FIELD")] | length > 0) or
         ([(.rhs1.access_path // [])[] | select(.kind == "FIELD")] | length > 0)
     )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN with FIELD accessor (dot member access)"

# at least one ASSIGN with a DEREF accessor followed by a FIELD accessor (arrow operator)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     (.rhs1.access_path // []) |
     select(
         any(.[]; .kind == "DEREF") and
         any(.[]; .kind == "FIELD")
     )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN with DEREF+FIELD access path (arrow operator)"

# ADDRESS_OF accessor must appear (a.next = &b)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(
         ([(.rhs1.access_path // [])[] | select(.kind == "ADDRESS_OF")] | length > 0)
     )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected ADDRESS_OF accessor (taking address of struct)"
