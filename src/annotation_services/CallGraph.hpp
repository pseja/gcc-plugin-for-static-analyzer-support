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
