#!/bin/bash

# @file 010_union.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests union members all share offset 0
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

# UNION type named 'data' must exist
jq -e '.types[] | select(.kind == "UNION" and .name == "data")' "$JSON" >/dev/null ||
	fail "union 'data' not found in types"

# all three members must appear in variables with byte_offset == 0
for member in i c str; do
	jq -e --arg n "$member" '.variables[] | select(.name == $n)' "$JSON" >/dev/null ||
		fail "union member '$member' not found in variables"
	OFFSET=$(jq -r --arg n "$member" '.variables[] | select(.name == $n) | .byte_offset // 0' "$JSON" | head -1)
	[ "$OFFSET" -eq 0 ] || fail "union member '$member' byte_offset should be 0, got $OFFSET"
done
