#!/bin/bash

# @file 035_callgraph.c.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests that call_graph annotation exists with correct roots/leaves/call edges for a simple call graph
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

# all four functions must be present
for fn in foo bar baz main; do
	jq -e --arg n "$fn" '.functions[] | select(.name == $n)' "$JSON" >/dev/null ||
		fail "function '$fn' not found"
done

# call_graph annotation must exist
jq -e '.annotations.call_graph' "$JSON" >/dev/null ||
	fail "call_graph annotation not found"

# foo is a leaf (no outgoing calls)
FOO_ID=$(jq '.functions[] | select(.name == "foo") | .id' "$JSON")
N=$(jq --argjson id "$FOO_ID" '
    [.annotations.call_graph.nodes[] |
     select(.function_id == $id) | .outgoing_calls[]] | length' "$JSON")
[ "$N" -eq 0 ] || fail "'foo' should have no outgoing calls (leaf), got $N"

# main is a root (no incoming calls)
MAIN_ID=$(jq '.functions[] | select(.name == "main") | .id' "$JSON")
N=$(jq --argjson id "$MAIN_ID" '
    [.annotations.call_graph.nodes[] |
     select(.function_id == $id) | .incoming_calls[]] | length' "$JSON")
[ "$N" -eq 0 ] || fail "'main' should have no incoming calls (root), got $N"

# foo appears as callee in at least 3 edges (called by main, baz, bar)
N=$(jq --argjson id "$FOO_ID" '
    [.annotations.call_graph.nodes[].outgoing_calls[] | select(.callee == $id)] | length' "$JSON")
[ "$N" -ge 3 ] || fail "expected 'foo' to appear as callee at least 3 times, got $N"

# roots array contains main's id
jq -e --argjson id "$MAIN_ID" '
    .annotations.call_graph.roots | index($id) != null' \
	"$JSON" >/dev/null || fail "main should be in call_graph.roots"

# leaves array contains foo's id
jq -e --argjson id "$FOO_ID" '
    .annotations.call_graph.leaves | index($id) != null' \
	"$JSON" >/dev/null || fail "foo should be in call_graph.leaves"
