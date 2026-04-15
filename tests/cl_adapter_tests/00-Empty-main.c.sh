#!/usr/bin/env bash
# @file 00-Empty-main.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests exactly one function named "main", no non-artificial variables (except labels)
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

# no non-artificial variables (except variables starting with 'L' which are generated labels)
jq -e '[.variables[] | select(.scope == "FUNCTION" and .artificial == false and (.name | startswith("L") | not))] | length == 0' "$JSON" >/dev/null

# at least one RETURN instruction
jq -e '[.instructions[] | select(.kind == "RETURN")] | length >= 1' "$JSON" >/dev/null
