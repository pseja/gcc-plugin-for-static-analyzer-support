#!/usr/bin/env bash
# @file 002_recursion_check_direct.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that recursion_check correctly identifies direct recursion (reuses test 017_recursion.c from integration_tests/).
# @date 2026-04-18
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
REPORT="$1"
fail() {
	echo "FAIL: $*" >&2
	exit 1
}

# must contain at least one direct-recursion entry
grep -q "^\[direct\]" "$REPORT" || fail "no [direct] entry found in report"

# the directly recursive function must be 'factorial'
grep -q "^\[direct\].*factorial" "$REPORT" || fail "'factorial' not listed as directly recursive"

# must not claim no recursion was found
! grep -q "^\[none\]" "$REPORT" || fail "report incorrectly claims no recursion"
