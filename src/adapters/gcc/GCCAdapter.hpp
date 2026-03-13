#pragma once

#include <gcc-plugin.h>

#include <unordered_map> // std::unordered_map

#include "CodeModel.hpp"
#include "DiagnosticReporter.hpp"
#include "FunctionId.hpp"
#include "SourceLocation.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

class GCCAdapter
{
  public:
    GCCAdapter(Core::CodeModel &model, Core::DiagnosticReporter &reporter);

    void processFunction(function *fun);

  private:
    Core::CodeModel &model;
    Core::DiagnosticReporter &reporter;

    std::string current_function_name;

    std::unordered_map<tree, Core::VariableId> variable_cache;
    std::unordered_map<tree, Core::TypeId> type_cache;
    std::unordered_map<basic_block, Core::BlockId> block_cache;

    Core::TypeId getOrCreateType(tree type_tree);
    Core::VariableId getOrCreateVariable(tree var_tree);
    Core::Operand parseOperand(tree operand_tree);

    Core::SourceLocation getSourceLocation(location_t location);

    Core::TypeKind mapTypeTreeToTypeKind(tree &type_tree);

    void processBlock(basic_block bb, Core::FunctionId function_id);
};

} // namespace CodeListener::CompilerAbstractionLayer
