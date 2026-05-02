/**
 * @file CallGraph.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the call graph annotation computed from a CodeModel.
 * @date 2026-05-02
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <unordered_map>
#include <vector>

#include "AnnotationBase.hpp"
#include "CallGraphNode.hpp"
#include "CodeModel.hpp"
#include "FunctionId.hpp"

namespace CodeListener::AnnotationServices
{

struct CallGraph : public AnnotationBase<CallGraph>
{
    std::unordered_map<Core::FunctionId, CallGraphNode> nodes;

    // functions with no incoming edges
    std::vector<Core::FunctionId> roots;
    // functions with no outgoing edges
    std::vector<Core::FunctionId> leaves;

    // topological sort of the graph represented as a vector of Strongly Connected Components (SCCs)
    // - a standard function is an SCC of size 1
    // - mutually recursive functions are grouped in the same inner vector
    std::vector<std::vector<Core::FunctionId>> topological_order;

    // global flags
    bool has_indirect_calls{false};
    bool has_callbacks{false};

    static CallGraph build(const Core::CodeModel &model);

  private:
    static void buildGraph(CallGraph &graph, const Core::CodeModel &model);
    static void computeRootsAndLeaves(CallGraph &graph);
    // computes the topological order of strongly connected components (SCCs) using Tarjan's algorithm
    static void computeTopologicalOrder(
        CallGraph &graph); // Kosaraju's algorithm is also an option (TODO: rate these in thesis)
};

} // namespace CodeListener::AnnotationServices
