#pragma once

#include <deque>
#include <string>
#include <unordered_map>

#include "CodeModel.hpp"
#include "../../../predator/include/cl/code_listener.h"

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

    std::deque<std::string> string_pool;

    std::unordered_map<Core::TypeId, struct cl_type *> type_map;
    std::unordered_map<Core::VariableId, struct cl_var *> var_map;

    struct cl_type *findType(const Core::Type *type);
    struct cl_var *findVariable(const Core::Variable *var);
    struct cl_operand mapOperand(const Core::Operand &op);
    struct cl_loc mapLocation(const Core::SourceLocation &loc);
    // TODO: implement
    struct cl_accessor *mapAccessor(const Core::Operand &op);

    // TODO: implement
    void emitGlobals();
    void emitFunctions();
    void emitFunction(const Core::Function &func);
    void emitInstruction(const Core::Instruction &inst);

    enum cl_binop_e mapBinOp(Core::OpCode op);
    int next_artificial_var_uid = 1000000;

    const char *persistString(const std::string &str);
};

} // namespace CodeListener::Adapters
