#!/usr/bin/env bash
# @file 001_callgraph_dot.sh
# @author Lukáš Pšeja <xpsejal00@vutbr.cz>
# @brief Tests the cl_analyze + JSONFrontend pipeline with the callgraph_dot analyzer.
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
DOT="$1"
fail() {
	echo "FAIL: $*" >&2
	exit 1
}

# output must be a valid DOT digraph
grep -q "^digraph" "$DOT" || fail "output is not a DOT digraph"

# all four functions must appear as nodes
for fn in foo bar baz main; do
	grep -q "\"${fn}\"" "$DOT" || fail "node '${fn}' not found in DOT output"
done

# expected call edges
grep -q '"bar".*->.*"foo"' "$DOT" || fail "edge bar->foo not found"
grep -q '"baz".*->.*"bar"' "$DOT" || fail "edge baz->bar not found"
grep -q '"main".*->.*"baz"' "$DOT" || fail "edge main->baz not found"

# foo should have no outgoing edges (leaf)
! grep -q '"foo".*->' "$DOT" || fail "foo should have no outgoing edges"
