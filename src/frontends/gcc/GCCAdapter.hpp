/**
 * @file GCCAdapter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares translation from GCC trees and GIMPLE into the compiler-independent CodeModel.
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <gcc-plugin.h>
#include <tree.h>            // all_warnings for gimple.h
#include <gimple.h>          // gimple_code
#include <gimple-iterator.h> // gimple_stmt_iterator

#pragma GCC diagnostic pop

#include <unordered_map> // std::unordered_map

#include "Block.hpp"
#include "CodeModel.hpp"
#include "DiagnosticReporter.hpp"
#include "FunctionId.hpp"
#include "Initializer.hpp"
#include "SourceLocation.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Translates GCC trees and GIMPLE statements into the compiler-independent CodeModel.
 */
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

    // synthetic type and variable cache for GCC internal functions
    Core::TypeId builtin_fn_type_id;
    std::unordered_map<std::string, Core::VariableId> internal_fn_cache;

    Core::TypeId getOrCreateType(tree type_tree);

    Core::Initializer parseInitializer(tree init_tree);

    Core::VariableId getOrCreateVariable(tree var_tree);
    Core::Operand parseOperand(tree operand_tree);

    location_t enhanceLocationT(location_t location, enum gimple_code gcode, gimple *stmt);
    Core::SourceLocation getSourceLocation(location_t location);

    Core::TypeKind mapTypeTreeToTypeKind(tree &type_tree);

    void processFunctionParameters(function *fun, Core::Function *function);

    void linkPredecessorsAndSuccessors(basic_block bb, Core::Block *block);

    void registerBasicBlock(basic_block bb);
    void registerBasicBlocks(function *fun);

    void processBasicBlock(basic_block bb, Core::FunctionId function_id);
    void processBasicBlocks(function *fun, Core::Function *function);

    void processInstruction(gimple *stmt, Core::Block *block);
    void processInstructions(basic_block bb, Core::Block *block);
};

} // namespace CodeListener::CompilerAbstractionLayer
