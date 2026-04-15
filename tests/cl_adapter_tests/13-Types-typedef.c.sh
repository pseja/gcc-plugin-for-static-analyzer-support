#!/usr/bin/env bash
# @file 13-Types-typedef.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that typedef aliases resolve to underlying kinds
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

jq -e '[.types[] | select(.name == "char_t" and .kind == "INTEGER")] | length == 1' "$JSON" >/dev/null
jq -e '[.types[] | select(.name == "int_t" and .kind == "INTEGER")] | length == 1' "$JSON" >/dev/null
jq -e '[.types[] | select(.name == "float_t" and .kind == "REAL")] | length == 1' "$JSON" >/dev/null
jq -e '[.types[] | select(.name == "struct_t" and .kind == "STRUCT")] | length == 1' "$JSON" >/dev/null
jq -e '[.types[] | select(.name == "union_t" and .kind == "UNION")] | length == 1' "$JSON" >/dev/null
