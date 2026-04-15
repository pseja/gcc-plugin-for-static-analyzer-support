#!/bin/bash

# @file 027_function_pointers.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests FUNCTION type, POINTER-to-FUNCTION type, indirect CALL
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

# functions 'add', 'sub' and 'main' exist
for fn in add sub main; do
	jq -e --arg n "$fn" '.functions[] | select(.name == $n)' "$JSON" >/dev/null ||
		fail "function '$fn' not found"
done

# FUNCTION type with exactly 2 parameter_type_ids must exist
jq -e '.types[] | select(.kind == "FUNCTION" and (.parameter_type_ids | length) == 2)' \
	"$JSON" >/dev/null || fail "expected a FUNCTION type with 2 parameters"

# POINTER-to-FUNCTION type must exist
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "POINTER") |
          select($t[.pointee_type_id|tostring].kind == "FUNCTION"))' \
	"$JSON" >/dev/null || fail "expected a POINTER-to-FUNCTION type"

# main contains at least one CALL instruction
N=$(jq '[.instructions[] | select(.kind == "CALL")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one CALL instruction (indirect call via pointer)"
