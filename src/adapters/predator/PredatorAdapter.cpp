#include "PredatorAdapter.hpp"

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

    if (listener->file_open)
    {
        listener->file_open(listener, persistString(model.getFilename()));
    }

    emitFunctions();

    if (listener->acknowledge)
    {
        listener->acknowledge(listener);
    }

    if (listener->file_close)
    {
        listener->file_close(listener);
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
        struct cl_operand cl_func_op
        {
        };
        cl_func_op.code = CL_OPERAND_CST;
        cl_func_op.type = const_cast<struct cl_type *>(mapType(model.getType(func.return_type_id)));

        listener->fnc_open(listener, &cl_func_op);
    }

    if (listener->fnc_arg_decl)
    {
        int arg_id = 0;
        for (const auto &arg_id_val : func.parameter_ids)
        {
            Core::VariableOperand var_op{arg_id_val, {}};
            Core::Operand op = var_op;

            cl_operands_pool.push_back(mapOperand(op));
            listener->fnc_arg_decl(listener, arg_id++, &cl_operands_pool.back());
        }
    }

    for (const auto &bbId : func.block_ids)
    {
        const auto *bb = model.getBlock(bbId);
        if (listener->bb_open)
        {
            listener->bb_open(listener, persistString(bb->name));
        }

        for (const auto &instId : bb->instruction_ids)
        {
            const auto *inst = model.getInstruction(instId);
            if (inst)
            {
                emitInstruction(*inst);
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
    struct cl_insn cl_i
    {
    };
    cl_i.loc = mapLocation(inst.source_location);

    if (auto *assign = std::get_if<Core::AssignInstruction>(&inst.data))
    {
        if (!assign->rhs1.has_value())
        {
            return;
        }

        cl_operands_pool.push_back(mapOperand(assign->lhs));
        const struct cl_operand *dst_op = &cl_operands_pool.back();

        cl_operands_pool.push_back(mapOperand(*assign->rhs1));
        const struct cl_operand *src1_op = &cl_operands_pool.back();

        const struct cl_operand *src2_op = nullptr;
        if (assign->rhs2.has_value())
        {
            cl_operands_pool.push_back(mapOperand(*assign->rhs2));
            src2_op = &cl_operands_pool.back();
        }

        bool is_unop = (!assign->rhs2.has_value() && assign->opcode != Core::OpCode::NONE) ||
                       assign->opcode == Core::OpCode::NONE || assign->opcode == Core::OpCode::CAST ||
                       assign->opcode == Core::OpCode::NEGATE || assign->opcode == Core::OpCode::BIT_NOT ||
                       assign->opcode == Core::OpCode::LOG_NOT || assign->opcode == Core::OpCode::ABS;

        if (is_unop)
        {
            cl_i.code = CL_INSN_UNOP;
            cl_i.data.insn_unop.dst = dst_op;
            cl_i.data.insn_unop.src = src1_op;

            switch (assign->opcode)
            {
            case Core::OpCode::NONE:
            case Core::OpCode::CAST:
                cl_i.data.insn_unop.code = CL_UNOP_ASSIGN;
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

            switch (assign->opcode)
            {
            case Core::OpCode::ADD:
                cl_i.data.insn_binop.code = CL_BINOP_PLUS;
                break;
            case Core::OpCode::SUB:
                cl_i.data.insn_binop.code = CL_BINOP_MINUS;
                break;
            case Core::OpCode::MUL:
                cl_i.data.insn_binop.code = CL_BINOP_MULT;
                break;
            case Core::OpCode::DIV:
                cl_i.data.insn_binop.code = CL_BINOP_TRUNC_DIV;
                break;
            case Core::OpCode::MOD:
                cl_i.data.insn_binop.code = CL_BINOP_TRUNC_MOD;
                break;
            case Core::OpCode::BIT_AND:
                cl_i.data.insn_binop.code = CL_BINOP_BIT_AND;
                break;
            case Core::OpCode::BIT_OR:
                cl_i.data.insn_binop.code = CL_BINOP_BIT_IOR;
                break;
            case Core::OpCode::BIT_XOR:
                cl_i.data.insn_binop.code = CL_BINOP_BIT_XOR;
                break;
            case Core::OpCode::SHL:
                cl_i.data.insn_binop.code = CL_BINOP_LSHIFT;
                break;
            case Core::OpCode::SHR:
                cl_i.data.insn_binop.code = CL_BINOP_RSHIFT;
                break;
            case Core::OpCode::LOG_AND:
                cl_i.data.insn_binop.code = CL_BINOP_TRUTH_AND;
                break;
            case Core::OpCode::LOG_OR:
                cl_i.data.insn_binop.code = CL_BINOP_TRUTH_OR;
                break;
            case Core::OpCode::EQUAL:
                cl_i.data.insn_binop.code = CL_BINOP_EQ;
                break;
            case Core::OpCode::NOT_EQUAL:
                cl_i.data.insn_binop.code = CL_BINOP_NE;
                break;
            case Core::OpCode::GREATER_THAN:
                cl_i.data.insn_binop.code = CL_BINOP_GT;
                break;
            case Core::OpCode::LESS_THAN:
                cl_i.data.insn_binop.code = CL_BINOP_LT;
                break;
            case Core::OpCode::GREATER_EQUAL:
                cl_i.data.insn_binop.code = CL_BINOP_GE;
                break;
            case Core::OpCode::LESS_EQUAL:
                cl_i.data.insn_binop.code = CL_BINOP_LE;
                break;
            case Core::OpCode::POINTER_ADD:
                cl_i.data.insn_binop.code = CL_BINOP_POINTER_PLUS;
                break;
            case Core::OpCode::POINTER_SUB:
                cl_i.data.insn_binop.code = CL_BINOP_POINTER_MINUS;
                break;
            case Core::OpCode::MIN:
                cl_i.data.insn_binop.code = CL_BINOP_MIN;
                break;
            case Core::OpCode::MAX:
                cl_i.data.insn_binop.code = CL_BINOP_MAX;
                break;
            case Core::OpCode::ROTATE_LEFT:
                cl_i.data.insn_binop.code = CL_BINOP_LROTATE;
                break;
            case Core::OpCode::ROTATE_RIGHT:
                cl_i.data.insn_binop.code = CL_BINOP_RROTATE;
                break;
            default:
                cl_i.data.insn_binop.code = CL_BINOP_PLUS;
                break;
            }
        }

        if (listener->insn)
        {
            listener->insn(listener, &cl_i);
        }
    }
    else if (auto *call = std::get_if<Core::CallInstruction>(&inst.data))
    {
        if (!listener->insn_call_open)
        {
            return;
        }

        cl_operands_pool.push_back(mapOperand(call->callee));
        const struct cl_operand *fnc_op = &cl_operands_pool.back();

        const struct cl_operand *dst_op = nullptr;
        if (call->lhs.has_value())
        {
            cl_operands_pool.push_back(mapOperand(*call->lhs));
            dst_op = &cl_operands_pool.back();
        }

        listener->insn_call_open(listener, &cl_i.loc, dst_op, fnc_op);

        if (listener->insn_call_arg)
        {
            int arg_idx = 0;
            for (const auto &arg : call->arguments)
            {
                cl_operands_pool.push_back(mapOperand(arg));
                listener->insn_call_arg(listener, arg_idx++, &cl_operands_pool.back());
            }
        }

        if (listener->insn_call_close)
        {
            listener->insn_call_close(listener);
        }
    }
    else if (auto *jmp = std::get_if<Core::GotoInstruction>(&inst.data))
    {
        cl_i.code = CL_INSN_JMP;
        const auto *target_bb = model.getBlock(jmp->target);
        cl_i.data.insn_jmp.label = target_bb ? persistString(target_bb->name) : nullptr;
        if (listener->insn)
        {
            listener->insn(listener, &cl_i);
        }
    }
    else if (auto *cond = std::get_if<Core::CondInstruction>(&inst.data))
    {
        cl_i.code = CL_INSN_COND;

        cl_operands_pool.push_back(mapOperand(cond->lhs));
        cl_i.data.insn_cond.src = &cl_operands_pool.back();

        const auto *true_bb = model.getBlock(cond->true_target);
        cl_i.data.insn_cond.then_label = true_bb ? persistString(true_bb->name) : nullptr;

        const auto *false_bb = model.getBlock(cond->false_target);
        cl_i.data.insn_cond.else_label = false_bb ? persistString(false_bb->name) : nullptr;

        if (listener->insn)
        {
            listener->insn(listener, &cl_i);
        }
    }
    else if (auto *ret = std::get_if<Core::ReturnInstruction>(&inst.data))
    {
        cl_i.code = CL_INSN_RET;
        if (ret->return_value)
        {
            cl_operands_pool.push_back(mapOperand(*ret->return_value));
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
    }
    else if (auto *sw = std::get_if<Core::SwitchInstruction>(&inst.data))
    {
        if (!listener->insn_switch_open)
        {
            return;
        }

        cl_operands_pool.push_back(mapOperand(sw->index));
        const struct cl_operand *src_op = &cl_operands_pool.back();

        listener->insn_switch_open(listener, &cl_i.loc, src_op);

        if (listener->insn_switch_case)
        {
            for (const auto &c : sw->cases)
            {
                const struct cl_operand *val_lo = nullptr;
                const struct cl_operand *val_hi = nullptr;

                if (c.low_value.has_value())
                {
                    cl_operands_pool.push_back(mapOperand(*c.low_value));
                    val_lo = &cl_operands_pool.back();
                }
                if (c.high_value.has_value())
                {
                    cl_operands_pool.push_back(mapOperand(*c.high_value));
                    val_hi = &cl_operands_pool.back();
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
    }
    else if (auto *clobber = std::get_if<Core::ClobberInstruction>(&inst.data))
    {
        cl_i.code = CL_INSN_CLOBBER;
        cl_operands_pool.push_back(mapOperand(clobber->clobbered_variable));
        cl_i.data.insn_clobber.var = &cl_operands_pool.back();
        if (listener->insn)
        {
            listener->insn(listener, &cl_i);
        }
    }
}

const char *PredatorAdapter::persistString(const std::string &str)
{
    string_pool.push_back(str);
    return string_pool.back().c_str();
}

struct cl_loc PredatorAdapter::mapLocation(const Core::SourceLocation &loc)
{
    struct cl_loc cl_l
    {
    };
    cl_l.file = persistString(loc.file);
    cl_l.line = loc.line;
    cl_l.column = loc.column;
    cl_l.sysp = false;
    return cl_l;
}

struct cl_type *PredatorAdapter::mapType(const Core::Type *type)
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

    cl_types_pool.push_back(cl_type{});
    struct cl_type *cl_t = &cl_types_pool.back();
    type_map[type->id] = cl_t;

    cl_t->uid = static_cast<cl_uid_t>(type->id.index);
    cl_t->name = persistString(type->name);
    cl_t->size = type->size_bits / 8;

    switch (type->kind)
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

    return cl_t;
}

struct cl_var *PredatorAdapter::mapVariable(const Core::Variable *var)
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

    cl_vars_pool.push_back(cl_var{});
    struct cl_var *cl_v = &cl_vars_pool.back();
    var_map[var->id] = cl_v;

    cl_v->uid = static_cast<cl_uid_t>(var->id.index);
    cl_v->name = persistString(var->name);
    cl_v->artificial = var->artificial;
    cl_v->loc = mapLocation(var->source_location);

    return cl_v;
}

struct cl_operand PredatorAdapter::mapOperand(const Core::Operand &op)
{
    struct cl_operand cl_op
    {
    };
    if (auto *const_op = std::get_if<Core::ConstantOperand>(&op))
    {
        cl_op.code = CL_OPERAND_CST;
        cl_op.type = mapType(model.getType(const_op->type_id));
        cl_op.data.cst.code = cl_op.type->code;

        if (cl_op.type->code == CL_TYPE_INT)
        {
            if (cl_op.type->is_unsigned)
            {
                cl_op.data.cst.data.cst_uint.value = std::strtoul(const_op->value.c_str(), nullptr, 10);
            }
            else
            {
                cl_op.data.cst.data.cst_int.value = std::strtol(const_op->value.c_str(), nullptr, 10);
            }
        }
        else if (cl_op.type->code == CL_TYPE_REAL)
        {
            cl_op.data.cst.data.cst_real.value = std::strtod(const_op->value.c_str(), nullptr);
        }
        else
        {
            cl_op.data.cst.data.cst_string.value = persistString(const_op->value);
        }
    }
    else if (auto *var_op = std::get_if<Core::VariableOperand>(&op))
    {
        cl_op.code = CL_OPERAND_VAR;
        const auto *var = model.getVariable(var_op->id);
        cl_op.type = mapType(model.getType(var->type_id));
        cl_op.data.var = mapVariable(var);

        if (!var_op->access_path.empty())
        {
            struct cl_accessor *head = nullptr;
            struct cl_accessor *tail = nullptr;
            for (const auto &acc : var_op->access_path)
            {
                cl_accessors_pool.push_back(cl_accessor{});
                struct cl_accessor *cl_a = &cl_accessors_pool.back();
                if (std::holds_alternative<Core::DerefAccessor>(acc.data))
                {
                    cl_a->code = CL_ACCESSOR_DEREF;
                }
                else if (std::holds_alternative<Core::AddressOfAccessor>(acc.data))
                {
                    cl_a->code = CL_ACCESSOR_REF;
                }
                else if (auto *arr_acc = std::get_if<Core::ArrayAccessor>(&acc.data))
                {
                    cl_a->code = CL_ACCESSOR_DEREF_ARRAY;
                    cl_operands_pool.push_back(mapOperand(arr_acc->index));
                    cl_a->data.array.index = &cl_operands_pool.back();
                }
                else if (auto *field_acc = std::get_if<Core::FieldAccessor>(&acc.data))
                {
                    cl_a->code = CL_ACCESSOR_ITEM;
                    cl_a->data.item.id = static_cast<int>(field_acc->field_id.index);
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

} // namespace CodeListener::Adapters
