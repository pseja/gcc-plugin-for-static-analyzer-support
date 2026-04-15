#!/bin/bash

# @file 039_inline_asm.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests ASM instructions carry template, operand constraints, clobbers
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

# function 'add_one' and 'main' exist
jq -e '.functions[] | select(.name == "add_one")' "$JSON" >/dev/null ||
	fail "function 'add_one' not found"
jq -e '.functions[] | select(.name == "main")' "$JSON" >/dev/null ||
	fail "function 'main' not found"

# at least 2 ASM instructions total
N=$(jq '[.instructions[] | select(.kind == "ASM")] | length' "$JSON")
[ "$N" -ge 2 ] || fail "expected >= 2 ASM instructions, got $N"

# every ASM must have assembly_string
jq -e '[.instructions[] | select(.kind == "ASM") | has("assembly_string")] | all' \
	"$JSON" >/dev/null || fail "at least one ASM instruction is missing 'assembly_string'"

# asm in add_one: 1 output, 1 input, >= 1 clobber, output constraint starts with '='
ADD_ONE_ID=$(jq '.functions[] | select(.name == "add_one") | .id' "$JSON")
ADD_ONE_BLOCK_IDS=$(jq -r --argjson fid "$ADD_ONE_ID" '[.blocks[] | select(.parent == $fid) | .id] | @json' "$JSON")

NOUT=$(jq --argjson bids "$ADD_ONE_BLOCK_IDS" \
	'first(.instructions[] | select(.kind == "ASM") |
            select( .parent_block_id as $b | $bids | index($b) != null )) |
    .outputs | length' "$JSON")
[ "$NOUT" -eq 1 ] || fail "add_one asm should have 1 output operand, got $NOUT"

NINP=$(jq --argjson bids "$ADD_ONE_BLOCK_IDS" \
	'first(.instructions[] | select(.kind == "ASM") |
            select( .parent_block_id as $b | $bids | index($b) != null )) |
    .inputs | length' "$JSON")
[ "$NINP" -eq 1 ] || fail "add_one asm should have 1 input operand, got $NINP"

NCLOB=$(jq --argjson bids "$ADD_ONE_BLOCK_IDS" \
	'first(.instructions[] | select(.kind == "ASM") |
            select( .parent_block_id as $b | $bids | index($b) != null )) |
    .clobbers | length' "$JSON")
[ "$NCLOB" -ge 1 ] || fail "add_one asm should have >= 1 clobber, got $NCLOB"

OUT_CONSTRAINT=$(jq -r --argjson bids "$ADD_ONE_BLOCK_IDS" \
	'first(.instructions[] | select(.kind == "ASM") |
            select( .parent_block_id as $b | $bids | index($b) != null )) |
    .outputs[0].constraint' "$JSON")
echo "$OUT_CONSTRAINT" | grep -q '^=' ||
	fail "output constraint should start with '=', got '$OUT_CONSTRAINT'"

# the volatile nop in main: is_volatile == true, clobbers 'memory'
MAIN_ID=$(jq '.functions[] | select(.name == "main") | .id' "$JSON")
MAIN_BLOCK_IDS=$(jq -r --argjson fid "$MAIN_ID" '[.blocks[] | select(.parent == $fid) | .id] | @json' "$JSON")

IS_VOL=$(jq --argjson bids "$MAIN_BLOCK_IDS" \
	'first(.instructions[] | select(.kind == "ASM") |
            select( .parent_block_id as $b | $bids | index($b) != null )) |
    .is_volatile' "$JSON")
[ "$IS_VOL" = "true" ] || fail "the __volatile__ nop asm should have is_volatile == true"

HIT=$(jq --argjson bids "$MAIN_BLOCK_IDS" \
	'first(.instructions[] | select(.kind == "ASM") |
            select( .parent_block_id as $b | $bids | index($b) != null )) |
    [.clobbers[] | select(. == "memory")] | length' "$JSON")
[ "$HIT" -ge 1 ] || fail "nop asm should clobber 'memory'"
