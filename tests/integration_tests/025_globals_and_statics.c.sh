#!/bin/bash

# @file 025_globals_and_statics.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests scope/storage/linkage for globals and statics
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

# function 'counter' and 'main' exist
jq -e '.functions[] | select(.name == "counter")' "$JSON" >/dev/null ||
	fail "function 'counter' not found"
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# global_var: GLOBAL scope, STATIC storage, EXTERNAL linkage
jq -e '.variables[] | select(.name == "global_var" and .scope == "GLOBAL")' \
	"$JSON" >/dev/null || fail "'global_var' with scope GLOBAL not found"
jq -e '.variables[] | select(.name == "global_var" and .scope == "GLOBAL" and
        .storage_duration == "STATIC" and .linkage == "EXTERNAL")' \
	"$JSON" >/dev/null ||
	fail "global_var should have STATIC storage and EXTERNAL linkage"

# count (static local in counter): STATIC scope, STATIC storage, INTERNAL linkage
jq -e '.variables[] | select(.name == "count" and .scope == "STATIC")' \
	"$JSON" >/dev/null || fail "static local 'count' with scope STATIC not found"
jq -e '.variables[] | select(.name == "count" and .scope == "STATIC" and
        .storage_duration == "STATIC" and .linkage == "INTERNAL")' \
	"$JSON" >/dev/null ||
	fail "count should have STATIC storage and INTERNAL linkage"
