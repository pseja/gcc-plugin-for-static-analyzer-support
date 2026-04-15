#!/usr/bin/env bash
# @file 05-Local-variable-with-initialization.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests a FUNCTION-scope variable "local_var" with AUTO storage and an initial value.
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

# one variable named "local_var" with FUNCTION scope
jq -e '[.variables[] | select(.name == "local_var" and .scope == "FUNCTION")] | length == 1' "$JSON" >/dev/null

# at least one ASSIGN instruction with a constant RHS value of 42
jq -e '[.instructions[] | select(.kind == "ASSIGN" and .rhs1.type == "constant" and .rhs1.value == "42")] | length >= 1' "$JSON" >/dev/null
