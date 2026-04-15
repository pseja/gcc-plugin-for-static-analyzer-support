#!/bin/bash

# @file 032_volatile.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that volatile int variable has is_volatile == true on its type
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

# variable 'hardware_register' exists
jq -e '.variables[] | select(.name == "hardware_register")' "$JSON" >/dev/null ||
	fail "variable 'hardware_register' not found"

# its type must have is_volatile == true
jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.variables[] | select(.name == "hardware_register") |
          select($t[.type_id|tostring].is_volatile == true))' \
	"$JSON" >/dev/null || fail "'hardware_register' type should have is_volatile == true"

# variable 'read_back' exists and its type should NOT be volatile
jq -e '.variables[] | select(.name == "read_back")' "$JSON" >/dev/null ||
	fail "variable 'read_back' not found"

jq -e '
    (.types | map({(.id|tostring): .}) | add) as $t |
    first(.variables[] | select(.name == "read_back") |
          select($t[.type_id|tostring].is_volatile != true))' \
	"$JSON" >/dev/null || fail "'read_back' type should not be volatile"

# ASSIGN instructions exist (the reads and writes to the volatile register)
N=$(jq '[.instructions[] | select(.kind == "ASSIGN")] | length' "$JSON")
[ "$N" -ge 2 ] || fail "expected at least 2 ASSIGN instructions for volatile read/write"
