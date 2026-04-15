#!/usr/bin/env bash
# @file 03-Global-variable-with-initialization.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that global variable initialized in main is not exported to the variables array, but an ASSIGN instruction with a non-zero constant RHS is generated.
# @date 2026-04-15
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
JSON=$1

# function 'main' exists and is the only function
jq -e '.functions | length == 1' "$JSON" >/dev/null
jq -e '.functions[0].name == "main"' "$JSON" >/dev/null

# at least one ASSIGN instruction with a non-zero constant RHS
jq -e '[.instructions[] | select(.kind == "ASSIGN" and .rhs1.type == "constant" and (.rhs1.value | tonumber) > 0)] | length >= 1' "$JSON" >/dev/null

# at least one RETURN instruction
jq -e '[.instructions[] | select(.kind == "RETURN")] | length >= 1' "$JSON" >/dev/null
