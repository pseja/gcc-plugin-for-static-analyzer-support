/**
 * @file PredatorAdapter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares conversion from the CodeModel to legacy Predator listener callbacks.
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

#include <deque>
#include <string>
#include <unordered_map>

#include "CodeModel.hpp"
#include "../../../analyzers/predator/include/cl/code_listener.h"

namespace CodeListener::Adapters
{

/** Replays the modern CodeModel through the historical Predator listener interface. */
class PredatorAdapter
{
  public:
    /**
     * Constructs an adapter over the completed model and the legacy listener callbacks.
     *
     * @param model Model to replay.
     * @param listener Legacy listener receiving converted callbacks.
     */
    PredatorAdapter(const Core::CodeModel &model, struct cl_code_listener *listener);

    /** Default destructor is sufficient because all pools are self-owned containers. */
    ~PredatorAdapter() = default;

    /** Emit the entire model through the legacy listener API. */
    void emit();

  private:
    /** Model being replayed through the legacy API. */
    const Core::CodeModel &model;

    /** Legacy listener receiving converted callbacks. */
    struct cl_code_listener *listener;

    /** Stable storage for converted legacy type records. */
    std::deque<struct cl_type> cl_types_pool;

    /** Stable storage for converted legacy variable records. */
    std::deque<struct cl_var> cl_vars_pool;

    /** Stable storage for converted legacy operand records. */
    std::deque<struct cl_operand> cl_operands_pool;

    /** Stable storage for converted legacy accessor records. */
    std::deque<struct cl_accessor> cl_accessors_pool;

    /** Stable storage for arrays of legacy type items referenced by aggregate types. */
    std::deque<std::vector<struct cl_type_item>> cl_type_items_pool;

    /** Stable storage for converted initializer chains. */
    std::deque<struct cl_initializer> cl_initializer_pool;

    /** Stable storage for strings whose C pointers are handed to the legacy API. */
    std::deque<std::string> string_pool;

    /** Mapping from model type ids to converted legacy type records. */
    std::unordered_map<Core::TypeId, struct cl_type *> type_map;

    /** Mapping from model variable ids to converted legacy variable records. */
    std::unordered_map<Core::VariableId, struct cl_var *> var_map;

    /** Mapping from function names to synthetic legacy function UIDs. */
    std::unordered_map<std::string, int> name_to_func_uid;

    /** Find or create the legacy representation of a model type. */
    struct cl_type *findType(const Core::Type *type);

    /** Find or create the legacy representation of a model variable. */
    struct cl_var *findVariable(const Core::Variable *var);

    /** Convert one model operand to the legacy operand structure. */
    struct cl_operand mapOperand(const Core::Operand &op);

    /** Convert one model source location to the legacy location structure. */
    struct cl_loc mapLocation(const Core::SourceLocation &loc);

    /** Build a legacy initializer chain from a model initializer subtree. */
    struct cl_initializer *buildInitializerChain(const Core::Initializer &init, struct cl_var *dst_var,
                                                 const struct cl_type *dst_type,
                                                 std::vector<std::pair<const struct cl_type *, int>> &field_path);

    /** Emit all functions in the model through the listener. */
    void emitFunctions();

    /** Emit one function and its blocks through the listener. */
    void emitFunction(const Core::Function &func);

    /** Emit one instruction through the listener. */
    void emitInstruction(const Core::Instruction &inst);

    /** Map a model opcode to the corresponding legacy binary-operator enum. */
    enum cl_binop_e mapBinOp(Core::OpCode op);

    /** Starting point for synthetic UIDs assigned to artificial variables. */
    int next_artificial_var_uid = 1000000;

    /** Persist a C++ string in `string_pool` and return its stable C string pointer. */
    const char *persistString(const std::string &str);
};

} // namespace CodeListener::Adapters
