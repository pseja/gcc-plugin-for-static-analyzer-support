#!/bin/bash

# @file 033_void_pointers.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests void* generic pointer, POINTER-to-VOID type, cast to int* produces POINTER-to-INTEGER, and DEREF accessor for *int_ptr
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

# POINTER-to-VOID type exists (the void*)
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "POINTER") |
          select($t[.pointee_type_id|tostring].kind == "VOID"))' \
	"$JSON" >/dev/null || fail "no POINTER-to-VOID type found (void*)"

# POINTER-to-INTEGER type also exists (the int* after cast)
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "POINTER") |
          select($t[.pointee_type_id|tostring].kind == "INTEGER"))' \
	"$JSON" >/dev/null || fail "no POINTER-to-INTEGER type found (int* cast)"

# ADDRESS_OF accessor (generic_ptr = &val)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(([(.rhs1.access_path // [])[] | select(.kind == "ADDRESS_OF")] | length > 0))] |
    length' "$JSON")
[ "$N" -ge 1 ] || fail "expected ADDRESS_OF accessor (taking address of val)"

# DEREF accessor (*int_ptr)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(
         ([(.lhs.access_path  // [])[] | select(.kind == "DEREF")] | length > 0) or
         ([(.rhs1.access_path // [])[] | select(.kind == "DEREF")] | length > 0)
     )] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected DEREF accessor (*int_ptr dereference)"
