#!/bin/bash

# @file 017_recursion.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests factorial calls itself, has one int parameter
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

# function 'factorial' and 'main' exist
jq -e '.functions[] | select(.name == "factorial")' "$JSON" >/dev/null ||
	fail "function 'factorial' not found"
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# factorial has exactly 1 parameter
N=$(jq '.functions[] | select(.name == "factorial") | .parameter_ids | length' "$JSON")
[ "$N" -eq 1 ] || fail "factorial should have 1 parameter, got $N"

# parameter is an INTEGER
PKIND=$(jq -r '
    (.variables | map({(.id|tostring): .}) | add) as $vars |
    (.types    | map({(.id|tostring): .}) | add) as $types |
    .functions[] | select(.name == "factorial") |
    .parameter_ids[0] as $vid |
    $types[ $vars[$vid|tostring].type_id | tostring ].kind' "$JSON")
[ "$PKIND" = "INTEGER" ] || fail "factorial parameter should be INTEGER, got $PKIND"

# factorial contains a CALL (the recursive call)
N=$(jq '[.instructions[] | select(.kind == "CALL")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one CALL instruction (recursive call)"
