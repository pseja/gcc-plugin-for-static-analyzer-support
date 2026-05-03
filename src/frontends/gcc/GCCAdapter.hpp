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
    /**
     * Constructs an adapter over the model being built and the active diagnostic sink.
     *
     * @param model Target model populated by the adapter.
     * @param reporter Diagnostic sink used for adapter warnings and failures.
     */
    GCCAdapter(Core::CodeModel &model, Core::DiagnosticReporter &reporter);

    /**
     * Translate one GCC function body into the CodeModel.
     *
     * @param fun GCC function currently processed by the custom pass.
     */
    void processFunction(function *fun);

  private:
    /** Model receiving translated entities and instructions. */
    Core::CodeModel &model;

    /** Diagnostic sink used while translating GCC IR. */
    Core::DiagnosticReporter &reporter;

    /** Name of the function currently being translated. */
    std::string current_function_name;

    /** Cache mapping GCC variable trees to already created model variables. */
    std::unordered_map<tree, Core::VariableId> variable_cache;

    /** Cache mapping GCC type trees to already created model types. */
    std::unordered_map<tree, Core::TypeId> type_cache;

    /** Cache mapping GCC basic blocks to already created model blocks. */
    std::unordered_map<basic_block, Core::BlockId> block_cache;

    /** Identifier of the synthesized function type reused for GCC internal helpers. */
    Core::TypeId builtin_fn_type_id;

    /** Cache of synthesized model variables representing GCC internal functions. */
    std::unordered_map<std::string, Core::VariableId> internal_fn_cache;

    /** Map one GCC type tree to a stable model type id, creating it if needed. */
    Core::TypeId getOrCreateType(tree type_tree);

    /** Parse a GCC initializer tree into the model initializer representation. */
    Core::Initializer parseInitializer(tree init_tree);

    /** Map one GCC variable tree to a stable model variable id, creating it if needed. */
    Core::VariableId getOrCreateVariable(tree var_tree);

    /** Parse a GCC operand tree into the model operand representation. */
    Core::Operand parseOperand(tree operand_tree);

    /** Improve a GCC location value using statement-specific fallback logic. */
    location_t enhanceLocationT(location_t location, enum gimple_code gcode, gimple *stmt);

    /** Convert a GCC location value into the model source location form. */
    Core::SourceLocation getSourceLocation(location_t location);

    /** Map one GCC type tree to the coarse `TypeKind` classification. */
    Core::TypeKind mapTypeTreeToTypeKind(tree &type_tree);

    /** Emit parameter variables for the current function. */
    void processFunctionParameters(function *fun, Core::Function *function);

    /** Compute predecessor and successor relations for a translated basic block. */
    void linkPredecessorsAndSuccessors(basic_block bb, Core::Block *block);

    /** Ensure one GCC basic block has a corresponding model block. */
    void registerBasicBlock(basic_block bb);

    /** Register all GCC basic blocks of one function before instruction emission starts. */
    void registerBasicBlocks(function *fun);

    /** Translate one GCC basic block into the model. */
    void processBasicBlock(basic_block bb, Core::FunctionId function_id);

    /** Translate all basic blocks of one function into the model. */
    void processBasicBlocks(function *fun, Core::Function *function);

    /** Translate one GCC statement into a model instruction. */
    void processInstruction(gimple *stmt, Core::Block *block);

    /** Translate all statements of one GCC basic block. */
    void processInstructions(basic_block bb, Core::Block *block);
};

} // namespace CodeListener::CompilerAbstractionLayer
