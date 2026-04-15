#!/bin/bash

# @file 023_bitfields.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that bitfield_size is set for all annotated fields
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

# STRUCT type named 'Flags' must exist
jq -e '.types[] | select(.kind == "STRUCT" and .name == "Flags")' "$JSON" >/dev/null ||
	fail "struct 'Flags' not found in types"

# named bit-fields must appear as variables with bitfield_size > 0
for field in is_ready status_code mode; do
	jq -e --arg n "$field" '.variables[] | select(.name == $n)' "$JSON" >/dev/null ||
		fail "bit-field '$field' not found in variables"
	BS=$(jq -r --arg n "$field" '.variables[] | select(.name == $n) | .bitfield_size // empty' "$JSON" | head -1)
	[ -n "$BS" ] || fail "field '$field' should have bitfield_size set"
	[ "$BS" -gt 0 ] || fail "field '$field' bitfield_size should be > 0, got $BS"
done

# is_ready == 1 bit
BS=$(jq '.variables[] | select(.name == "is_ready") | .bitfield_size' "$JSON" | head -1)
[ "$BS" -eq 1 ] || fail "is_ready bitfield_size should be 1, got $BS"

# status_code == 3 bits
BS=$(jq '.variables[] | select(.name == "status_code") | .bitfield_size' "$JSON" | head -1)
[ "$BS" -eq 3 ] || fail "status_code bitfield_size should be 3, got $BS"
