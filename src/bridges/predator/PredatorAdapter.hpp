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

class PredatorAdapter
{
  public:
    PredatorAdapter(const Core::CodeModel &model, struct cl_code_listener *listener);
    ~PredatorAdapter() = default;

    void emit();

  private:
    const Core::CodeModel &model;
    struct cl_code_listener *listener;

    std::deque<struct cl_type> cl_types_pool;
    std::deque<struct cl_var> cl_vars_pool;
    std::deque<struct cl_operand> cl_operands_pool;
    std::deque<struct cl_accessor> cl_accessors_pool;
    std::deque<std::vector<struct cl_type_item>> cl_type_items_pool;
    std::deque<struct cl_initializer> cl_initializer_pool;

    std::deque<std::string> string_pool;

    std::unordered_map<Core::TypeId, struct cl_type *> type_map;
    std::unordered_map<Core::VariableId, struct cl_var *> var_map;
    std::unordered_map<std::string, int> name_to_func_uid;

    struct cl_type *findType(const Core::Type *type);
    struct cl_var *findVariable(const Core::Variable *var);
    struct cl_operand mapOperand(const Core::Operand &op);
    struct cl_loc mapLocation(const Core::SourceLocation &loc);

    // build cl_initializer chain from Core::Initializer, appending to **tail
    // returns pointer to first node (or nullptr if no initializers)
    struct cl_initializer *buildInitializerChain(const Core::Initializer &init, struct cl_var *dst_var,
                                                 const struct cl_type *dst_type,
                                                 std::vector<std::pair<const struct cl_type *, int>> &field_path);

    void emitFunctions();
    void emitFunction(const Core::Function &func);
    void emitInstruction(const Core::Instruction &inst);

    enum cl_binop_e mapBinOp(Core::OpCode op);
    int next_artificial_var_uid = 1000000;

    const char *persistString(const std::string &str);
};

} // namespace CodeListener::Adapters
