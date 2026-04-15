#!/bin/bash

# @file 038_complex_types.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests COMPLEX kind, component is REAL, add_complex has 2 COMPLEX params
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

# function 'add_complex' and 'main' exist
jq -e '.functions[] | select(.name == "add_complex")' "$JSON" >/dev/null ||
	fail "function 'add_complex' not found"
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# at least one COMPLEX type
jq -e '.types[] | select(.kind == "COMPLEX")' "$JSON" >/dev/null ||
	fail "expected at least one COMPLEX type"

# COMPLEX type's component is REAL
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.types[] | select(.kind == "COMPLEX")) |
    select($t[.component_type_id|tostring].kind == "REAL")' \
	"$JSON" >/dev/null || fail "COMPLEX type component should be REAL (double)"

# add_complex has exactly 2 parameters
N=$(jq '.functions[] | select(.name == "add_complex") | .parameter_ids | length' "$JSON")
[ "$N" -eq 2 ] || fail "add_complex should have 2 parameters, got $N"

# both parameters are of COMPLEX type
KINDS=$(jq -r '
    (.variables | map({(.id|tostring): .}) | add) as $vars |
    (.types    | map({(.id|tostring): .}) | add) as $types |
    [.functions[] | select(.name == "add_complex") | .parameter_ids[] |
     $types[ $vars[tostring].type_id | tostring ].kind] | unique | join(",")' "$JSON")
[ "$KINDS" = "COMPLEX" ] || fail "add_complex param kinds should all be COMPLEX, got $KINDS"
