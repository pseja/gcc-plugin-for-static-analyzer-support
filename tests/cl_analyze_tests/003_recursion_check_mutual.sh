#!/usr/bin/env bash
# @file 003_recursion_check_mutual.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that recursion_check correctly identifies mutual recursion.
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

# must contain at least one mutual-recursion entry
grep -q '^\[mutual\]' "$REPORT" || fail "no [mutual] entry found in report"

# both "even" and "odd" must appear on the mutual line
grep '^\[mutual\]' "$REPORT" | grep -q 'even' || fail '"even" not in mutual group'
grep '^\[mutual\]' "$REPORT" | grep -q 'odd' || fail '"odd" not in mutual group'

# must not claim no recursion was found
! grep -q '^\[none\]' "$REPORT" || fail "report incorrectly claims no recursion"
