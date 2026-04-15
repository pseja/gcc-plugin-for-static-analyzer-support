#!/usr/bin/env bash
# @file 07-Types-int.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that standard integer types (short, int, long) have correct sizes in bits.
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

jq -e '[.types[] | select(.kind == "INTEGER" and .name == "int" and .size_bits == 32)] | length >= 1' "$JSON" >/dev/null
jq -e '[.types[] | select(.kind == "INTEGER" and (.name == "short int" or .name == "short") and .size_bits == 16)] | length >= 1' "$JSON" >/dev/null
jq -e '[.types[] | select(.kind == "INTEGER" and .name == "long int" and .size_bits == 64)] | length >= 1' "$JSON" >/dev/null
