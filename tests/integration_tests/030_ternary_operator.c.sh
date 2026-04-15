#!/bin/bash

# @file 030_ternary_operator.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests ternary (a > b) ? a : b lowers to an ASSIGN with two rhs operands (MAX/MIN style selection in GIMPLE)
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

# ternary lowering produces an ASSIGN with both rhs1 and rhs2 populated
# (GIMPLE turns (a > b) ? a : b into a two-operand MAX-like assign)
N=$(jq '
    [.instructions[] | select(.kind == "ASSIGN") |
     select(.rhs1 != null and .rhs2 != null)] |
    length' "$JSON")
[ "$N" -ge 1 ] || fail "expected an ASSIGN with both rhs1 and rhs2 (ternary lowering)"

# both rhs operands should be variable references (not constants)
jq -e '
    first(.instructions[] | select(.kind == "ASSIGN") |
          select(.rhs1.type == "variable" and .rhs2.type == "variable"))' \
	"$JSON" >/dev/null || fail "ternary ASSIGN should have two variable operands (a and b)"

# at least the variables a, b, max are present
jq -e '.variables[] | select(.name == "a")' "$JSON" >/dev/null || fail "variable 'a' not found"
jq -e '.variables[] | select(.name == "b")' "$JSON" >/dev/null || fail "variable 'b' not found"
jq -e '.variables[] | select(.name == "max")' "$JSON" >/dev/null || fail "variable 'max' not found"
