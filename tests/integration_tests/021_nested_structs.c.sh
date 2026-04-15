#!/bin/bash

# @file 021_nested_structs.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests structs Engine and Car, nested FIELD accessor chain
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

# both struct types must be present
jq -e '.types[] | select(.kind == "STRUCT" and .name == "Engine")' "$JSON" >/dev/null ||
	fail "struct 'Engine' not found in types"
jq -e '.types[] | select(.kind == "STRUCT" and .name == "Car")' "$JSON" >/dev/null ||
	fail "struct 'Car' not found in types"

# at least one ASSIGN whose lhs access_path has >= 2 FIELD steps (nested member access)
N=$(jq '[ .instructions[] | select(.kind == "ASSIGN") |
           ([ (.lhs.access_path // [])[] | select(.kind == "FIELD") ] | length) ] |
         max' "$JSON")
[ "$N" -ge 2 ] || fail "expected at least one ASSIGN with 2+ nested FIELD accessors, max was $N"
