#!/bin/bash

# @file 015_switch.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests SWITCH instruction with cases including a range and a default
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

# at least one SWITCH instruction
N=$(jq '[.instructions[] | select(.kind == "SWITCH")] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one SWITCH instruction"

# SWITCH has 'index' and 'cases' fields
jq -e '.instructions[] | select(.kind == "SWITCH") | has("index") and has("cases")' \
	"$JSON" >/dev/null || fail "SWITCH instruction missing 'index' or 'cases' field"

# >= 3 cases
N=$(jq 'first(.instructions[] | select(.kind == "SWITCH")) | .cases | length' "$JSON")
[ "$N" -ge 3 ] || fail "expected >= 3 switch cases, got $N"

# at least one range case (has both low_value and high_value)
N=$(jq 'first(.instructions[] | select(.kind == "SWITCH")) |
        [.cases[] | select(has("low_value") and has("high_value"))] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one range case with low_value and high_value"

# at least one default case (no low_value)
N=$(jq 'first(.instructions[] | select(.kind == "SWITCH")) |
        [.cases[] | select(has("low_value") | not)] | length' "$JSON")
[ "$N" -ge 1 ] || fail "expected at least one default case (no low_value)"
