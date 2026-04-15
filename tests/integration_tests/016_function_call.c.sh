#!/bin/bash

# @file 016_function_call.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests call semantics, param types, return value
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

# function 'foo' and 'main' exist
jq -e '.functions[] | select(.name == "foo")' "$JSON" >/dev/null ||
	fail "function 'foo' not found"
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# foo has exactly 2 parameters
N=$(jq '.functions[] | select(.name == "foo") | .parameter_ids | length' "$JSON")
[ "$N" -eq 2 ] || fail "foo should have 2 parameters, got $N"

# foos parameter types sorted should be [INTEGER, REAL]
KINDS=$(jq -r '
    (.variables | map({(.id|tostring): .}) | add) as $vars |
    (.types    | map({(.id|tostring): .}) | add) as $types |
    [.functions[] | select(.name == "foo") | .parameter_ids[] |
     $types[ $vars[tostring].type_id | tostring ].kind] | sort | join(",")' "$JSON")
[ "$KINDS" = "INTEGER,REAL" ] || fail "foo param kinds (sorted) should be INTEGER,REAL, got $KINDS"

# foos return type is REAL
RET_KIND=$(jq -r '
    (.types | map({(.id|tostring): .}) | add) as $types |
    .functions[] | select(.name == "foo") |
    $types[.return_type_id | tostring].kind' "$JSON")
[ "$RET_KIND" = "REAL" ] || fail "foo return type should be REAL, got $RET_KIND"

# main contains a CALL to foo
N=$(jq '[.instructions[] | select(.kind == "CALL")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one CALL instruction in main"

# call to foo captures a return value (lhs is not null)
jq -e '.instructions[] | select(.kind == "CALL") | select(.lhs != null)' \
	"$JSON" >/dev/null || fail "expected the CALL to foo to capture a return value (lhs != null)"
