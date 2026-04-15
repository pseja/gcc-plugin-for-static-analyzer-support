#!/bin/bash

# @file 037_anonymous_structs.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests synthesised <anon.*> names instead of empty strings
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

# no struct/union/enum type should have an empty name
N=$(jq '[.types[] | select(.kind == ("STRUCT","UNION","ENUM") and .name == "")] | length' "$JSON")
[ "$N" -eq 0 ] || {
	IDS=$(jq '[.types[] | select(.kind == ("STRUCT","UNION","ENUM") and .name == "") | .id] | @json' "$JSON")
	fail "found $N struct/union/enum type(s) with empty name: $IDS"
}

# named struct 'Wrapper' must exist
jq -e '.types[] | select(.kind == "STRUCT" and .name == "Wrapper")' "$JSON" >/dev/null ||
	fail "struct 'Wrapper' not found"

# anonymous struct should have an <anon.struct.*> name
jq -e '.types[] | select(.kind == "STRUCT" and (.name | startswith("<anon.struct.")))' \
	"$JSON" >/dev/null || fail "expected an anonymous struct with <anon.struct.*> name"

# anonymous union should have an <anon.union.*> name
jq -e '.types[] | select(.kind == "UNION" and (.name | startswith("<anon.union.")))' \
	"$JSON" >/dev/null || fail "expected an anonymous union with <anon.union.*> name"
