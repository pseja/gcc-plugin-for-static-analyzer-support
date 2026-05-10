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

/** Stores the derived call graph together with summary properties used by analyzers and exporters. */
struct CallGraph : public AnnotationBase<CallGraph>
{
    /** Per-function call graph nodes keyed by function identifier. */
    std::unordered_map<Core::FunctionId, CallGraphNode> nodes;

    /** Functions with no incoming call edges. */
    std::vector<Core::FunctionId> roots;

    /** Functions with no outgoing call edges. */
    std::vector<Core::FunctionId> leaves;

    /**
     * Topological order of the graph represented as strongly connected components.
     *
     * A non-recursive function forms an SCC of size 1, while mutually recursive functions share one inner vector.
     */
    std::vector<std::vector<Core::FunctionId>> topological_order;

    /** Whether the graph contains at least one indirect call edge. */
    bool has_indirect_calls{false};

    /** Whether the graph observed at least one function-pointer callback use. */
    bool has_callbacks{false};

    /**
     * Build the complete call graph annotation for the supplied model.
     *
     * @param model Model whose call relationships should be analyzed.
     *
     * @return Fully populated call graph annotation.
     */
    static CallGraph build(const Core::CodeModel &model);

  private:
    /**
     * Populate the raw node and edge structure of the graph.
     *
     * @param graph Graph to populate.
     * @param model CodeModel supplying call relationships.
     */
    static void buildGraph(CallGraph &graph, const Core::CodeModel &model);

    /**
     * Compute the sets of roots and leaves after the graph edges were built.
     *
     * @param graph Graph whose root/leaf sets should be computed.
     */
    static void computeRootsAndLeaves(CallGraph &graph);

    /**
     * Compute the SCC topological order using Tarjan's algorithm.
     *
     * @param graph Graph whose nodes should be topologically ordered.
     */
    static void computeTopologicalOrder(CallGraph &graph);
};

} // namespace CodeListener::AnnotationServices
