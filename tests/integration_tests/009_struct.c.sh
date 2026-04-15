#!/bin/bash

# @file 009_struct.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests struct type fields and member access
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

# STRUCT type named 'Point' must exist
jq -e '.types[] | select(.kind == "STRUCT" and .name == "Point")' "$JSON" >/dev/null ||
	fail "struct 'Point' not found in types"

# fields 'x' and 'y' appear as variables
jq -e '.variables[] | select(.name == "x")' "$JSON" >/dev/null ||
	fail "field 'x' not found in variables"
jq -e '.variables[] | select(.name == "y")' "$JSON" >/dev/null ||
	fail "field 'y' not found in variables"

# at least one ASSIGN with a FIELD accessor (struct member access)
N=$(jq '[ .instructions[] | select(.kind == "ASSIGN") |
           select( (.lhs.access_path // [])[] | .kind == "FIELD" ) ] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one ASSIGN with a FIELD accessor"
