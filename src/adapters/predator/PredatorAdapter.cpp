#include <variant>
#include <cstring>

#include "EnumType.hpp"
#include "FunctionType.hpp"
#include "PredatorAdapter.hpp"
#include "UnionType.hpp"
#include "utility.hpp"

using CodeListener::Exporters::overloaded;

namespace CodeListener::Adapters
{

PredatorAdapter::PredatorAdapter(const Core::CodeModel &model, struct cl_code_listener *listener)
    : model(model), listener(listener)
{
}

void PredatorAdapter::emit()
{
    if (!listener)
    {
        return;
    }

    // pre-passes for types and variables, because old cl expects to know them already
    for (const auto &type : model.getTypes())
    {
        cl_types_pool.emplace_back();
        struct cl_type *cl_t = &cl_types_pool.back();
        type_map[type.id] = cl_t;

        cl_t->uid = static_cast<int>(type.id.index);
        cl_t->name = persistString(type.name);
        cl_t->size = type.size_bits / 8;

        switch (type.kind)
        {
        case Core::TypeKind::VOID:
            cl_t->code = CL_TYPE_VOID;
            break;
        case Core::TypeKind::UNKNOWN:
            cl_t->code = CL_TYPE_UNKNOWN;
            break;
        case Core::TypeKind::POINTER:
            cl_t->code = CL_TYPE_PTR;
            break;
        case Core::TypeKind::STRUCT:
            cl_t->code = CL_TYPE_STRUCT;
            break;
        case Core::TypeKind::UNION:
            cl_t->code = CL_TYPE_UNION;
            break;
        case Core::TypeKind::ARRAY:
            cl_t->code = CL_TYPE_ARRAY;
            break;
        case Core::TypeKind::FUNCTION:
            cl_t->code = CL_TYPE_FNC;
            break;
        case Core::TypeKind::INTEGER:
            cl_t->code = CL_TYPE_INT;
            break;
        case Core::TypeKind::BOOL:
            cl_t->code = CL_TYPE_BOOL;
            break;
        case Core::TypeKind::ENUM:
            cl_t->code = CL_TYPE_ENUM;
            break;
        case Core::TypeKind::REAL:
            cl_t->code = CL_TYPE_REAL;
            break;
        default:
            cl_t->code = CL_TYPE_UNKNOWN;
            break;
        }
    }

    for (const auto &type : model.getTypes())
    {
        struct cl_type *cl_t = type_map[type.id];

        std::visit(overloaded{[&](const Core::ArrayType &at) {
                                  cl_t->item_cnt = 1;
                                  cl_type_items_pool.push_back(std::vector<cl_type_item>(1));
                                  cl_t->items = cl_type_items_pool.back().data();
                                  cl_t->items[0].type = type_map[at.element_type_id];
                              },
                              [&](const Core::FunctionType &ft) {
                                  cl_t->item_cnt = 1 + static_cast<int>(ft.parameter_type_ids.size());
                                  cl_type_items_pool.push_back(std::vector<cl_type_item>(cl_t->item_cnt));
                                  cl_t->items = cl_type_items_pool.back().data();
                                  cl_t->items[0].type = type_map[ft.return_type_id];
                                  for (size_t i = 0; i < ft.parameter_type_ids.size(); ++i)
                                  {
                                      cl_t->items[i + 1].type = type_map[ft.parameter_type_ids[i]];
                                  }
                              },
                              [&](const Core::PointerType &pt) {
                                  cl_t->item_cnt = 1;
                                  cl_type_items_pool.push_back(std::vector<cl_type_item>(1));
                                  cl_t->items = cl_type_items_pool.back().data();
                                  cl_t->items[0].type = type_map[pt.pointee_type_id];
                              },
                              [&](const Core::StructType &st) {
                                  cl_t->item_cnt = static_cast<int>(st.fields.size());
                                  cl_type_items_pool.push_back(std::vector<cl_type_item>(cl_t->item_cnt));
                                  cl_t->items = cl_type_items_pool.back().data();
                                  for (size_t i = 0; i < st.fields.size(); ++i)
                                  {
                                      const auto *field_var = model.getVariable(st.fields[i]);
                                      cl_t->items[i].type = type_map[field_var->type_id];
                                      cl_t->items[i].name = persistString(field_var->name);
                                      if (auto *fv = std::get_if<Core::FieldVariable>(&field_var->data))
                                      {
                                          cl_t->items[i].offset = fv->byte_offset.value_or(0);
                                      }
                                      else
                                      {
                                          cl_t->items[i].offset = 0;
                                      }
                                  }
                              },
                              [&](const Core::UnionType &st) {
                                  cl_t->item_cnt = static_cast<int>(st.fields.size());
                                  cl_type_items_pool.push_back(std::vector<cl_type_item>(cl_t->item_cnt));
                                  cl_t->items = cl_type_items_pool.back().data();
                                  for (size_t i = 0; i < st.fields.size(); ++i)
                                  {
                                      const auto *field_var = model.getVariable(st.fields[i]);
                                      cl_t->items[i].type = type_map[field_var->type_id];
                                      cl_t->items[i].name = persistString(field_var->name);
                                      if (auto *fv = std::get_if<Core::FieldVariable>(&field_var->data))
                                      {
                                          cl_t->items[i].offset = fv->byte_offset.value_or(0);
                                      }
                                      else
                                      {
                                          cl_t->items[i].offset = 0;
                                      }
                                  }
                              },
                              [&](const Core::IntegerType &it) { cl_t->is_unsigned = it.is_unsigned; },
                              [&](const Core::EnumType &et) { cl_t->is_unsigned = et.is_unsigned; },
                              [&](const auto &) { /* primitives have no items */ }},
                   type.data);
    }

    for (const auto &var : model.getVariables())
    {
        cl_vars_pool.emplace_back();
        struct cl_var *cl_v = &cl_vars_pool.back();
        var_map[var.id] = cl_v;

        cl_v->uid = static_cast<int>(var.id.index);
        cl_v->name = persistString(var.name);
        cl_v->artificial = var.artificial;
        cl_v->loc = mapLocation(var.source_location);

        if (const auto *sv = std::get_if<Core::StandardVariable>(&var.data))
        {
            cl_v->is_extern = (sv->storage_duration == Core::StorageDuration::EXTERN);
            cl_v->initialized = sv->initial_value.has_value();
        }
    }

    if (listener->file_open)
    {
        listener->file_open(listener, persistString(model.getFilename())); // TODO: Map main source file
    }

    emitFunctions();

    if (listener->file_close)
    {
        listener->file_close(listener);
    }

    if (listener->acknowledge)
    {
        listener->acknowledge(listener);
    }
}

void PredatorAdapter::emitFunctions()
{
    for (const auto &func : model.getFunctions())
    {
        emitFunction(func);
    }
}

void PredatorAdapter::emitFunction(const Core::Function &func)
{
    if (listener->fnc_open)
    {
        struct cl_operand cl_func_op{};
        cl_func_op.code = CL_OPERAND_CST;

        cl_types_pool.emplace_back();
        struct cl_type *cl_t = &cl_types_pool.back();
        cl_t->code = CL_TYPE_FNC;
        // functions also end up in VarDb as cl_var operands, offsetting their IDs
        cl_t->uid = static_cast<int>(func.id.index) + 1000000;
        cl_t->name = persistString(func.name);

        cl_type_items_pool.push_back(std::vector<cl_type_item>(1));
        struct cl_type_item *ret_item = &cl_type_items_pool.back().front();
        ret_item->type = findType(model.getType(func.return_type_id));
        ret_item->name = nullptr;
        ret_item->offset = 0;

        cl_t->item_cnt = 1;
        cl_t->items = ret_item;

        cl_func_op.type = cl_t;

        cl_func_op.data.cst.code = CL_TYPE_FNC;

        cl_func_op.data.cst.data.cst_fnc.uid = static_cast<int>(func.id.index) + 1000000;
        cl_func_op.data.cst.data.cst_fnc.name = persistString(func.name);
        cl_func_op.data.cst.data.cst_fnc.is_extern = false;

        struct cl_loc loc{};
        loc.file = persistString(model.getFilename());
        loc.line = 1;
        loc.column = 1;
        cl_func_op.data.cst.data.cst_fnc.loc = loc;

        listener->fnc_open(listener, &cl_func_op);
    }

    if (listener->fnc_arg_decl)
    {
        int arg_id = 1;
        for (const auto &arg_id_val : func.parameter_ids)
        {
            Core::VariableOperand var_op{arg_id_val, {}};
            Core::Operand op = var_op;

            cl_operands_pool.push_back(mapOperand(op));
            listener->fnc_arg_decl(listener, arg_id++, &cl_operands_pool.back());
        }
    }

    if (!func.block_ids.empty() && listener->insn)
    {
        const auto *entry_bb = model.getBlock(func.block_ids.front());
        struct cl_insn cl_i{};
        cl_i.code = CL_INSN_JMP;

        struct cl_loc loc{};
        loc.file = persistString(model.getFilename());
        loc.line = 1;
        loc.column = 1;

        cl_i.loc = loc;
        std::string target_label = entry_bb->name;
        if (target_label == "ENTRY" && !entry_bb->successors.empty())
        {
            const auto *succ_bb = model.getBlock(entry_bb->successors[0]);
            if (succ_bb)
            {
                target_label = succ_bb->name;
            }
        }
        cl_i.data.insn_jmp.label = persistString(target_label);
        listener->insn(listener, &cl_i);
    }

    for (const auto &bbId : func.block_ids)
    {
        const auto *bb = model.getBlock(bbId);
        if (bb->name == "ENTRY" || bb->name == "EXIT")
        {
            continue;
        }

        if (listener->bb_open)
        {
            listener->bb_open(listener, persistString(bb->name));
        }

        bool has_terminator = false;
        for (const auto &instId : bb->instruction_ids)
        {
            const auto *inst = model.getInstruction(instId);
            if (inst)
            {
                emitInstruction(*inst);
                if (std::holds_alternative<Core::GotoInstruction>(inst->data) ||
                    std::holds_alternative<Core::ReturnInstruction>(inst->data) ||
                    std::holds_alternative<Core::CondInstruction>(inst->data) ||
                    std::holds_alternative<Core::SwitchInstruction>(inst->data) ||
                    std::holds_alternative<Core::AbortInstruction>(inst->data))
                {
                    has_terminator = true;
                }
            }
        }

        if (!has_terminator && !bb->successors.empty())
        {
            const auto *succ_bb = model.getBlock(bb->successors[0]);
            struct cl_insn cl_i{};
            cl_loc loc{};
            loc.file = persistString(model.getFilename());
            loc.line = 1;
            loc.column = 1;

            if (succ_bb && succ_bb->name != "EXIT")
            {
                cl_i.code = CL_INSN_JMP;
                cl_i.loc = loc;
                cl_i.data.insn_jmp.label = persistString(succ_bb->name);
                if (listener->insn)
                {
                    listener->insn(listener, &cl_i);
                }
            }
            else if (succ_bb && succ_bb->name == "EXIT")
            {
                cl_i.code = CL_INSN_RET;
                cl_i.loc = loc;
                cl_i.data.insn_ret.src = nullptr;
                if (listener->insn)
                {
                    listener->insn(listener, &cl_i);
                }
            }
        }
    }

    if (listener->fnc_close)
    {
        listener->fnc_close(listener);
    }
}

void PredatorAdapter::emitInstruction(const Core::Instruction &inst)
{
    struct cl_insn cl_i{};
    cl_i.loc = mapLocation(inst.source_location);

    std::visit(
        overloaded{[&](const Core::AssignInstruction &assign) {
                       if (!assign.rhs1.has_value())
                       {
                           return;
                       }

                       cl_operands_pool.push_back(mapOperand(assign.lhs));
                       const struct cl_operand *dst_op = &cl_operands_pool.back();

                       cl_operands_pool.push_back(mapOperand(assign.rhs1.value()));
                       const struct cl_operand *src1_op = &cl_operands_pool.back();

                       const struct cl_operand *src2_op = nullptr;
                       if (assign.rhs2.has_value())
                       {
                           cl_operands_pool.push_back(mapOperand(assign.rhs2.value()));
                           src2_op = &cl_operands_pool.back();
                       }

                       bool is_unop = (!assign.rhs2.has_value() && assign.opcode != Core::OpCode::NONE) ||
                                      assign.opcode == Core::OpCode::NONE || assign.opcode == Core::OpCode::CAST ||
                                      assign.opcode == Core::OpCode::NEGATE || assign.opcode == Core::OpCode::BIT_NOT ||
                                      assign.opcode == Core::OpCode::LOG_NOT || assign.opcode == Core::OpCode::ABS;

                       if (is_unop)
                       {
                           cl_i.code = CL_INSN_UNOP;
                           cl_i.data.insn_unop.dst = dst_op;
                           cl_i.data.insn_unop.src = src1_op;

                           switch (assign.opcode)
                           {
                           case Core::OpCode::NONE:
                               cl_i.data.insn_unop.code = CL_UNOP_ASSIGN;
                               break;
                           case Core::OpCode::CAST:
                               if (dst_op->type && src1_op->type && dst_op->type->code == CL_TYPE_REAL &&
                                   src1_op->type->code == CL_TYPE_INT)
                               {
                                   cl_i.data.insn_unop.code = CL_UNOP_FLOAT;
                               }
                               else
                               {
                                   cl_i.data.insn_unop.code = CL_UNOP_ASSIGN;
                               }
                               break;
                           case Core::OpCode::NEGATE:
                               cl_i.data.insn_unop.code = CL_UNOP_MINUS;
                               break;
                           case Core::OpCode::BIT_NOT:
                               cl_i.data.insn_unop.code = CL_UNOP_BIT_NOT;
                               break;
                           case Core::OpCode::LOG_NOT:
                               cl_i.data.insn_unop.code = CL_UNOP_TRUTH_NOT;
                               break;
                           case Core::OpCode::ABS:
                               cl_i.data.insn_unop.code = CL_UNOP_ABS;
                               break;
                           default:
                               cl_i.data.insn_unop.code = CL_UNOP_ASSIGN;
                               break;
                           }
                       }
                       else
                       {
                           cl_i.code = CL_INSN_BINOP;
                           cl_i.data.insn_binop.dst = dst_op;
                           cl_i.data.insn_binop.src1 = src1_op;
                           cl_i.data.insn_binop.src2 = src2_op;

                           cl_i.data.insn_binop.code = mapBinOp(assign.opcode);
                       }

                       if (listener->insn)
                       {
                           listener->insn(listener, &cl_i);
                       }
                   },
                   [&](const Core::CallInstruction &call) {
                       if (!listener->insn_call_open)
                       {
                           return;
                       }

                       cl_operands_pool.push_back(mapOperand(call.callee));
                       const struct cl_operand *fnc_op = &cl_operands_pool.back();

                       const struct cl_operand *dst_op = nullptr;
                       if (call.lhs.has_value())
                       {
                           cl_operands_pool.push_back(mapOperand(*call.lhs));
                           dst_op = &cl_operands_pool.back();
                       }

                       listener->insn_call_open(listener, &cl_i.loc, dst_op, fnc_op);

                       if (listener->insn_call_arg)
                       {
                           int arg_idx = 1;
                           for (const auto &arg : call.arguments)
                           {
                               cl_operands_pool.push_back(mapOperand(arg));
                               listener->insn_call_arg(listener, arg_idx++, &cl_operands_pool.back());
                           }
                       }

                       if (listener->insn_call_close)
                       {
                           listener->insn_call_close(listener);
                       }
                   },
                   [&](const Core::GotoInstruction &jmp) {
                       cl_i.code = CL_INSN_JMP;
                       const auto *target_bb = model.getBlock(jmp.target);
                       cl_i.data.insn_jmp.label = target_bb ? persistString(target_bb->name) : nullptr;
                       if (listener->insn)
                       {
                           listener->insn(listener, &cl_i);
                       }
                   },
                   [&](const Core::CondInstruction &cond) {
                       if (cond.opcode != Core::OpCode::NONE)
                       {
                           struct cl_var fake_var{};
                           fake_var.uid = next_artificial_var_uid++;
                           fake_var.name = nullptr;
                           fake_var.artificial = true;
                           fake_var.is_extern = false;
                           fake_var.initialized = true;

                           cl_vars_pool.push_back(fake_var);
                           struct cl_var *dst_var = &cl_vars_pool.back();

                           cl_operands_pool.push_back(mapOperand(cond.lhs));
                           const struct cl_operand *src1_op = &cl_operands_pool.back();

                           cl_operands_pool.push_back(mapOperand(cond.rhs));
                           const struct cl_operand *src2_op = &cl_operands_pool.back();

                           struct cl_operand dst_op{};
                           dst_op.code = CL_OPERAND_VAR;
                           dst_op.type = src1_op->type;
                           dst_op.data.var = dst_var;

                           cl_operands_pool.push_back(dst_op);
                           const struct cl_operand *dst_op_ptr = &cl_operands_pool.back();

                           struct cl_insn binop_i{};
                           binop_i.loc = cl_i.loc;
                           binop_i.code = CL_INSN_BINOP;
                           binop_i.data.insn_binop.dst = dst_op_ptr;
                           binop_i.data.insn_binop.src1 = src1_op;
                           binop_i.data.insn_binop.src2 = src2_op;
                           binop_i.data.insn_binop.code = mapBinOp(cond.opcode);

                           if (listener->insn)
                           {
                               listener->insn(listener, &binop_i);
                           }

                           cl_i.code = CL_INSN_COND;
                           cl_i.data.insn_cond.src = dst_op_ptr;
                       }
                       else
                       {
                           cl_i.code = CL_INSN_COND;
                           cl_operands_pool.push_back(mapOperand(cond.lhs));
                           cl_i.data.insn_cond.src = &cl_operands_pool.back();
                       }

                       const auto *true_bb = model.getBlock(cond.true_target);
                       cl_i.data.insn_cond.then_label = true_bb ? persistString(true_bb->name) : nullptr;

                       const auto *false_bb = model.getBlock(cond.false_target);
                       cl_i.data.insn_cond.else_label = false_bb ? persistString(false_bb->name) : nullptr;

                       if (listener->insn)
                       {
                           listener->insn(listener, &cl_i);
                       }
                   },
                   [&](const Core::ReturnInstruction &ret) {
                       cl_i.code = CL_INSN_RET;
                       if (ret.return_value)
                       {
                           cl_operands_pool.push_back(mapOperand(*ret.return_value));
                           cl_i.data.insn_ret.src = &cl_operands_pool.back();
                       }
                       else
                       {
                           cl_i.data.insn_ret.src = nullptr;
                       }
                       if (listener->insn)
                       {
                           listener->insn(listener, &cl_i);
                       }
                   },
                   [&](const Core::SwitchInstruction &sw) {
                       if (!listener->insn_switch_open)
                       {
                           return;
                       }

                       cl_operands_pool.push_back(mapOperand(sw.index));
                       const struct cl_operand *src_op = &cl_operands_pool.back();

                       listener->insn_switch_open(listener, &cl_i.loc, src_op);

                       if (listener->insn_switch_case)
                       {
                           for (const auto &c : sw.cases)
                           {
                               const struct cl_operand *val_lo = nullptr;
                               const struct cl_operand *val_hi = nullptr;

                               if (!c.low_value.has_value())
                               {
                                   // default case: the old CL requires CL_OPERAND_VOID for both lo and hi
                                   cl_operands_pool.emplace_back();
                                   cl_operands_pool.back().code = CL_OPERAND_VOID;
                                   val_lo = &cl_operands_pool.back();
                                   cl_operands_pool.emplace_back();
                                   cl_operands_pool.back().code = CL_OPERAND_VOID;
                                   val_hi = &cl_operands_pool.back();
                               }
                               else
                               {
                                   // regular or range case: both lo and hi must be CL_OPERAND_CST/CL_TYPE_INT
                                   cl_operands_pool.push_back(mapOperand(*c.low_value));
                                   val_lo = &cl_operands_pool.back();
                                   if (c.high_value.has_value())
                                   {
                                       cl_operands_pool.push_back(mapOperand(*c.high_value));
                                       val_hi = &cl_operands_pool.back();
                                   }
                                   else
                                   {
                                       // single case: hi == lo
                                       cl_operands_pool.push_back(mapOperand(*c.low_value));
                                       val_hi = &cl_operands_pool.back();
                                   }
                               }

                               const auto *target_bb = model.getBlock(c.target_block_id);
                               const char *label = target_bb ? persistString(target_bb->name) : nullptr;

                               listener->insn_switch_case(listener, &cl_i.loc, val_lo, val_hi, label);
                           }
                       }
                       if (listener->insn_switch_close)
                       {
                           listener->insn_switch_close(listener);
                       }
                   },
                   [&](const Core::ClobberInstruction &clobber) {
                       cl_i.code = CL_INSN_CLOBBER;
                       cl_operands_pool.push_back(mapOperand(clobber.clobbered_variable));
                       cl_i.data.insn_clobber.var = &cl_operands_pool.back();
                       if (listener->insn)
                       {
                           listener->insn(listener, &cl_i);
                       }
                   },
                   [&](const Core::LabelInstruction &label) {
                       if (auto *v_op = std::get_if<Core::VariableOperand>(&label.label))
                       {
                           if (const auto *var = model.getVariable(v_op->id))
                           {
                               if (var->name.find("L") != 0 && var->name.find("D.") != 0 && var->name.find("bb_") != 0)
                               {
                                   cl_i.code = CL_INSN_LABEL;
                                   cl_i.data.insn_label.name = persistString(var->name);
                                   if (listener->insn)
                                   {
                                       listener->insn(listener, &cl_i);
                                   }
                               }
                           }
                       }
                   },
                   [&](const Core::AbortInstruction &) {
                       cl_i.code = CL_INSN_ABORT;
                       if (listener->insn)
                       {
                           listener->insn(listener, &cl_i);
                       }
                   },
                   [&](const auto &) {
                       // unhandled instruction types
                   }},
        inst.data);
}

const char *PredatorAdapter::persistString(const std::string &str)
{
    string_pool.push_back(str);
    return string_pool.back().c_str();
}

struct cl_loc PredatorAdapter::mapLocation(const Core::SourceLocation &loc)
{
    struct cl_loc cl_l{};
    cl_l.file = persistString(loc.file);
    cl_l.line = loc.line;
    cl_l.column = loc.column;
    cl_l.sysp = false;
    return cl_l;
}

struct cl_type *PredatorAdapter::findType(const Core::Type *type)
{
    if (!type)
    {
        return nullptr;
    }
    auto it = type_map.find(type->id);
    if (it != type_map.end())
    {
        return it->second;
    }

    return nullptr;
}

struct cl_var *PredatorAdapter::findVariable(const Core::Variable *var)
{
    if (!var)
    {
        return nullptr;
    }
    auto it = var_map.find(var->id);
    if (it != var_map.end())
    {
        return it->second;
    }

    return nullptr;
}

struct cl_operand PredatorAdapter::mapOperand(const Core::Operand &op)
{
    struct cl_operand cl_op{};

    if (auto *const_op = std::get_if<Core::ConstantOperand>(&op))
    {
        if (!const_op->type_id.isValid())
        {
            cl_op.code = CL_OPERAND_VOID;
            return cl_op;
        }

        cl_op.code = CL_OPERAND_CST;
        cl_op.type = findType(model.getType(const_op->type_id));
        if (!cl_op.type)
        {
            cl_op.code = CL_OPERAND_VOID;
            return cl_op;
        }

        if (const_op->value.size() >= 2 && const_op->value.front() == '"')
        {
            // string literal: always stored as CL_TYPE_STRING regardless of pointer type
            cl_op.data.cst.code = CL_TYPE_STRING;
            std::string unquoted = const_op->value.substr(1, const_op->value.length() - 2);
            cl_op.data.cst.data.cst_string.value = persistString(unquoted);
        }
        else if (cl_op.type->code == CL_TYPE_REAL)
        {
            cl_op.data.cst.code = CL_TYPE_REAL;
            cl_op.data.cst.data.cst_real.value = std::strtod(const_op->value.c_str(), nullptr);
        }
        else
        {
            // INT, ENUM, BOOL, PTR, CHAR, UNKNOWN are all stored as integer
            cl_op.data.cst.code = CL_TYPE_INT;
            if (cl_op.type->is_unsigned)
            {
                cl_op.data.cst.data.cst_uint.value = std::strtoul(const_op->value.c_str(), nullptr, 10);
            }
            else
            {
                cl_op.data.cst.data.cst_int.value = std::strtol(const_op->value.c_str(), nullptr, 10);
            }
        }
    }
    else if (auto *var_op = std::get_if<Core::VariableOperand>(&op))
    {
        const auto *var = model.getVariable(var_op->id);
        if (!var || !var->type_id.isValid())
        {
            cl_op.code = CL_OPERAND_VOID;
            return cl_op;
        }
        const auto *type = model.getType(var->type_id);

        if (type && type->kind == Core::TypeKind::FUNCTION)
        {
            cl_op.code = CL_OPERAND_CST;
            cl_op.type = findType(type);
            cl_op.data.cst.code = CL_TYPE_FNC;

            cl_op.data.cst.data.cst_fnc.uid = static_cast<int>(var->id.index) + 1000000;
            cl_op.data.cst.data.cst_fnc.name = persistString(var->name);
            cl_op.data.cst.data.cst_fnc.is_extern = false; // FIXME: a guess for now

            return cl_op;
        }

        cl_op.code = CL_OPERAND_VAR;
        cl_op.type = findType(type);
        cl_op.data.var = findVariable(var);

        if (!var_op->access_path.empty())
        {
            struct cl_accessor *head = nullptr;
            struct cl_accessor *tail = nullptr;
            const struct cl_type *current_type = cl_op.type;

            for (const auto &acc : var_op->access_path)
            {
                cl_accessors_pool.emplace_back();
                struct cl_accessor *cl_a = &cl_accessors_pool.back();
                memset((void *)cl_a, 0, sizeof(*cl_a));

                cl_a->type = const_cast<struct cl_type *>(current_type);

                if (std::holds_alternative<Core::DerefAccessor>(acc.data))
                {
                    cl_a->code = CL_ACCESSOR_DEREF;
                    if (current_type && current_type->item_cnt > 0)
                        current_type = current_type->items[0].type;
                }
                else if (auto *addr_acc = std::get_if<Core::AddressOfAccessor>(&acc.data))
                {
                    cl_a->code = CL_ACCESSOR_REF;
                    current_type = findType(model.getType(addr_acc->target_type_id));
                }
                else if (auto *arr_acc = std::get_if<Core::ArrayAccessor>(&acc.data))
                {
                    cl_a->code = CL_ACCESSOR_DEREF_ARRAY;
                    cl_operands_pool.push_back(mapOperand(arr_acc->index));
                    cl_a->data.array.index = &cl_operands_pool.back();
                    if (current_type && current_type->item_cnt > 0)
                        current_type = current_type->items[0].type;
                }
                else if (auto *field_acc = std::get_if<Core::FieldAccessor>(&acc.data))
                {
                    cl_a->code = CL_ACCESSOR_ITEM;
                    const auto *field_var = model.getVariable(field_acc->field_id);
                    cl_a->data.item.id = 0;
                    if (current_type && field_var)
                    {
                        for (int i = 0; i < current_type->item_cnt; ++i)
                        {
                            if (current_type->items[i].name && field_var->name == current_type->items[i].name)
                            {
                                cl_a->data.item.id = i;
                                break;
                            }
                        }
                    }
                    if (current_type && cl_a->data.item.id < current_type->item_cnt)
                        current_type = current_type->items[cl_a->data.item.id].type;
                }
                else if (auto *off_acc = std::get_if<Core::OffsetAccessor>(&acc.data))
                {
                    cl_a->code = CL_ACCESSOR_OFFSET;
                    if (auto *const_op = std::get_if<Core::ConstantOperand>(&off_acc->offset))
                    {
                        cl_a->data.offset.off = std::strtol(const_op->value.c_str(), nullptr, 10);
                    }
                    else
                    {
                        cl_a->data.offset.off = 0;
                    }
                }

                if (!head)
                {
                    head = cl_a;
                }
                else
                {
                    tail->next = cl_a;
                }
                tail = cl_a;
            }
            cl_op.accessor = head;
        }
    }
    return cl_op;
}

enum cl_binop_e PredatorAdapter::mapBinOp(Core::OpCode op)
{
    switch (op)
    {
    case Core::OpCode::ADD:
        return CL_BINOP_PLUS;
    case Core::OpCode::SUB:
        return CL_BINOP_MINUS;
    case Core::OpCode::MUL:
        return CL_BINOP_MULT;
    case Core::OpCode::DIV:
        return CL_BINOP_TRUNC_DIV;
    case Core::OpCode::MOD:
        return CL_BINOP_TRUNC_MOD;
    case Core::OpCode::BIT_AND:
        return CL_BINOP_BIT_AND;
    case Core::OpCode::BIT_OR:
        return CL_BINOP_BIT_IOR;
    case Core::OpCode::BIT_XOR:
        return CL_BINOP_BIT_XOR;
    case Core::OpCode::SHL:
        return CL_BINOP_LSHIFT;
    case Core::OpCode::SHR:
        return CL_BINOP_RSHIFT;
    case Core::OpCode::LOG_AND:
        return CL_BINOP_TRUTH_AND;
    case Core::OpCode::LOG_OR:
        return CL_BINOP_TRUTH_OR;
    case Core::OpCode::EQUAL:
        return CL_BINOP_EQ;
    case Core::OpCode::NOT_EQUAL:
        return CL_BINOP_NE;
    case Core::OpCode::GREATER_THAN:
        return CL_BINOP_GT;
    case Core::OpCode::LESS_THAN:
        return CL_BINOP_LT;
    case Core::OpCode::GREATER_EQUAL:
        return CL_BINOP_GE;
    case Core::OpCode::LESS_EQUAL:
        return CL_BINOP_LE;
    case Core::OpCode::POINTER_ADD:
        return CL_BINOP_POINTER_PLUS;
    case Core::OpCode::POINTER_SUB:
        return CL_BINOP_POINTER_MINUS;
    case Core::OpCode::MIN:
        return CL_BINOP_MIN;
    case Core::OpCode::MAX:
        return CL_BINOP_MAX;
    case Core::OpCode::ROTATE_LEFT:
        return CL_BINOP_LROTATE;
    case Core::OpCode::ROTATE_RIGHT:
        return CL_BINOP_RROTATE;
    default:
        return CL_BINOP_PLUS;
    }
}

} // namespace CodeListener::Adapters
