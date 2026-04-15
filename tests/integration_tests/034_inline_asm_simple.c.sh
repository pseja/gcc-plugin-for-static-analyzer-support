#!/bin/bash

# @file 034_inline_asm.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that a single simple volatile ASM block with one output operand
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

# at least one ASM instruction
jq -e '.instructions[] | select(.kind == "ASM")' "$JSON" >/dev/null ||
	fail "no ASM instruction found"

# ASM instruction has an assembly_string field (non-empty for nop)
jq -e 'first(.instructions[] | select(.kind == "ASM") | .assembly_string | . != null)' \
	"$JSON" >/dev/null || fail "ASM instruction missing assembly_string"

# ASM is volatile (__asm__ __volatile__)
jq -e 'first(.instructions[] | select(.kind == "ASM") | select(.is_volatile == true))' \
	"$JSON" >/dev/null || fail "ASM instruction should be volatile"

# exactly one output operand ("=r"(result))
N=$(jq 'first(.instructions[] | select(.kind == "ASM")) | .outputs | length' "$JSON")
[ "$N" -eq 1 ] || fail "expected exactly 1 output operand for the nop asm, got $N"

# no input operands
N=$(jq 'first(.instructions[] | select(.kind == "ASM")) | .inputs | length' "$JSON")
[ "$N" -eq 0 ] || fail "expected 0 input operands for the nop asm, got $N"
