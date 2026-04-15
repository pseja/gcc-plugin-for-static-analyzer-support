#!/bin/bash

# @file 036_variadic_functions.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests is_variadic flag on FunctionType
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

# function 'sum', 'add' and 'main' exists
for fn in sum add main; do
	jq -e --arg n "$fn" '.functions[] | select(.name == $n)' "$JSON" >/dev/null ||
		fail "function '$fn' not found"
done

# locate the FUNCTION type for 'sum' via the variable named 'sum'
SUM_TID=$(jq '
    (.variables | map({(.id|tostring): .}) | add) as $vars |
    (.types    | map({(.id|tostring): .}) | add) as $types |
    .variables[] | select(.name == "sum") |
    .type_id as $tid |
    # walk through POINTER indirections to reach the FUNCTION type
    ($types[$tid|tostring] |
        if .kind == "POINTER" then $types[.pointee_type_id|tostring]
        else . end) |
    select(.kind == "FUNCTION") | .id' "$JSON" | head -1)
[ -n "$SUM_TID" ] || fail "could not find FunctionType for 'sum' via variable 'sum'"

IS_VAR=$(jq --argjson tid "$SUM_TID" '.types[] | select(.id == $tid) | .is_variadic' "$JSON")
[ "$IS_VAR" = "true" ] || fail "sum FunctionType.is_variadic should be true, got $IS_VAR"

# 'add' must NOT be variadic
ADD_TID=$(jq '
    (.variables | map({(.id|tostring): .}) | add) as $vars |
    (.types    | map({(.id|tostring): .}) | add) as $types |
    .variables[] | select(.name == "add") |
    .type_id as $tid |
    ($types[$tid|tostring] |
        if .kind == "POINTER" then $types[.pointee_type_id|tostring]
        else . end) |
    select(.kind == "FUNCTION") | .id' "$JSON" | head -1)
[ -n "$ADD_TID" ] || fail "could not find FunctionType for 'add'"

IS_VAR=$(jq --argjson tid "$ADD_TID" '.types[] | select(.id == $tid) | .is_variadic' "$JSON")
[ "$IS_VAR" = "false" ] || fail "add FunctionType.is_variadic should be false, got $IS_VAR"

# sum has exactly 1 declared parameter
N=$(jq '.functions[] | select(.name == "sum") | .parameter_ids | length' "$JSON")
[ "$N" -eq 1 ] || fail "sum should have 1 declared parameter, got $N"
