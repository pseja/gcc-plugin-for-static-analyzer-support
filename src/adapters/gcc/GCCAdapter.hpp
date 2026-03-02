#pragma once

#include <gcc-plugin.h>

#include <unordered_map> // std::unordered_map

#include "CodeModel.hpp"
#include "NodeId.hpp"
#include "SourceLocation.hpp"

namespace CodeListener
{

namespace Core
{

class GCCAdapter
{
  public:
    GCCAdapter(CodeModel &model);

    void processFunction(function *fun);

  private:
    CodeModel &model;

    std::string current_function_name;
    std::unordered_map<tree, NodeId> variable_cache;
    std::unordered_map<tree, NodeId> type_cache;

    NodeId getOrCreateType(tree type_tree);
    NodeId getOrCreateVariable(tree var_tree);
    Operand parseOperand(tree operand_tree);

    SourceLocation getSourceLocation(location_t location);

    TypeKind mapTypeTreeToTypeKind(tree &type_tree);

    void processBlock(basic_block bb, NodeId function_id);
};

} // namespace Core

} // namespace CodeListener
