#pragma once

#include <gcc-plugin.h>

#include <unordered_map> // std::unordered_map

#include "CodeModel.hpp"
#include "NodeId.hpp"
#include "SourceLocation.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

class GCCAdapter
{
  public:
    GCCAdapter(Core::CodeModel &model);

    void processFunction(function *fun);

  private:
    Core::CodeModel &model;

    std::string current_function_name;
    std::unordered_map<tree, Core::NodeId> variable_cache;
    std::unordered_map<tree, Core::NodeId> type_cache;

    Core::NodeId getOrCreateType(tree type_tree);
    Core::NodeId getOrCreateVariable(tree var_tree);
    Core::Operand parseOperand(tree operand_tree);

    Core::SourceLocation getSourceLocation(location_t location);

    Core::TypeKind mapTypeTreeToTypeKind(tree &type_tree);

    void processBlock(basic_block bb, Core::NodeId function_id);
};

} // namespace CodeListener::CompilerAbstractionLayer
