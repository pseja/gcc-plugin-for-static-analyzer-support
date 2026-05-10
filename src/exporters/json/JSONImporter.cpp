/**
 * @file JSONImporter.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the JSON deserializer for CodeModel instances.
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

#include <fstream>
#include <memory>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "JSONImporter.hpp"

#include "AccessorKind.hpp"
#include "AsmInstruction.hpp"
#include "AsmOperandConstraint.hpp"
#include "AssignInstruction.hpp"
#include "Block.hpp"
#include "CallInstruction.hpp"
#include "ClobberInstruction.hpp"
#include "CodeModel.hpp"
#include "ComplexType.hpp"
#include "CondInstruction.hpp"
#include "FieldVariable.hpp"
#include "Function.hpp"
#include "GotoInstruction.hpp"
#include "Initializer.hpp"
#include "Instruction.hpp"
#include "InstructionKind.hpp"
#include "LabelInstruction.hpp"
#include "Linkage.hpp"
#include "Operand.hpp"
#include "OpCode.hpp"
#include "PhiInstruction.hpp"
#include "PhiIncomingValue.hpp"
#include "ReturnInstruction.hpp"
#include "Scope.hpp"
#include "SourceLocation.hpp"
#include "StandardVariable.hpp"
#include "StorageDuration.hpp"
#include "SwitchCase.hpp"
#include "SwitchInstruction.hpp"
#include "Type.hpp"
#include "TypeData.hpp"
#include "TypeKind.hpp"
#include "Variable.hpp"
#include "UnknownInstruction.hpp"

/** Convenience alias for the nlohmann::json type used throughout this file. */
using json = nlohmann::json;

namespace CodeListener
{

namespace Core
{

/**
 * Deserialize a typed Id from a JSON value.
 *
 * @tparam T  Entity type the identifier refers to.
 * @param j   JSON value containing a numeric index or null.
 * @param id  Id object to populate.
 */
template <typename T>
void from_json(const json &j, Id<T> &id)
{
    if (j.is_null())
    {
        id.index = std::numeric_limits<unsigned long>::max();
    }
    else
    {
        id.index = j.get<unsigned long>();
    }
}

/**
 * Parse a SourceLocation from JSON.
 *
 * @param j JSON object with file, line, column, and function fields.
 *
 * @return Populated SourceLocation.
 */
static SourceLocation parseSourceLocation(const json &j)
{
    SourceLocation loc;
    loc.file = j.value("file", "");
    loc.line = j.value("line", 0u);
    loc.column = j.value("column", 0u);
    loc.function = j.value("function", "");
    return loc;
}

/**
 * Convert a string representation of TypeKind to the corresponding enum value.
 *
 * @param s TypeKind string (e.g. "INTEGER", "POINTER").
 *
 * @return Equivalent TypeKind enum value.
 */
static constexpr TypeKind parseTypeKind(const std::string &s) noexcept
{
    if (s == "ENUM")
    {
        return TypeKind::ENUM;
    }
    if (s == "BOOL")
    {
        return TypeKind::BOOL;
    }
    if (s == "INTEGER")
    {
        return TypeKind::INTEGER;
    }
    if (s == "REAL")
    {
        return TypeKind::REAL;
    }
    if (s == "POINTER")
    {
        return TypeKind::POINTER;
    }
    if (s == "ARRAY")
    {
        return TypeKind::ARRAY;
    }
    if (s == "STRUCT")
    {
        return TypeKind::STRUCT;
    }
    if (s == "UNION")
    {
        return TypeKind::UNION;
    }
    if (s == "VOID")
    {
        return TypeKind::VOID;
    }
    if (s == "FUNCTION")
    {
        return TypeKind::FUNCTION;
    }
    if (s == "COMPLEX")
    {
        return TypeKind::COMPLEX;
    }

    return TypeKind::UNKNOWN;
}

/**
 * Convert a string representation of InstructionKind to the corresponding enum value.
 *
 * @param s InstructionKind string (e.g. "ASSIGN", "CALL").
 *
 * @return Equivalent InstructionKind enum value.
 */
static constexpr InstructionKind parseInstructionKind(const std::string &s) noexcept
{
    if (s == "ASSIGN")
    {
        return InstructionKind::ASSIGN;
    }
    if (s == "CALL")
    {
        return InstructionKind::CALL;
    }
    if (s == "RETURN")
    {
        return InstructionKind::RETURN;
    }
    if (s == "COND")
    {
        return InstructionKind::COND;
    }
    if (s == "SWITCH")
    {
        return InstructionKind::SWITCH;
    }
    if (s == "GOTO")
    {
        return InstructionKind::GOTO;
    }
    if (s == "LABEL")
    {
        return InstructionKind::LABEL;
    }
    if (s == "ASM")
    {
        return InstructionKind::ASM;
    }
    if (s == "PHI")
    {
        return InstructionKind::PHI;
    }
    if (s == "NOP")
    {
        return InstructionKind::NOP;
    }
    if (s == "CLOBBER")
    {
        return InstructionKind::CLOBBER;
    }
    if (s == "UNREACHABLE")
    {
        return InstructionKind::UNREACHABLE;
    }
    if (s == "ABORT")
    {
        return InstructionKind::ABORT;
    }

    return InstructionKind::UNKNOWN;
}

/**
 * Convert a string representation of OpCode to the corresponding enum value.
 *
 * @param s OpCode string (e.g. "ADD", "CAST").
 *
 * @return Equivalent OpCode enum value.
 */
static constexpr OpCode parseOpCode(const std::string &s) noexcept
{
    if (s == "NONE")
    {
        return OpCode::NONE;
    }
    if (s == "NEGATE")
    {
        return OpCode::NEGATE;
    }
    if (s == "BIT_NOT")
    {
        return OpCode::BIT_NOT;
    }
    if (s == "LOG_NOT")
    {
        return OpCode::LOG_NOT;
    }
    if (s == "ABS")
    {
        return OpCode::ABS;
    }
    if (s == "ADD")
    {
        return OpCode::ADD;
    }
    if (s == "SUB")
    {
        return OpCode::SUB;
    }
    if (s == "MUL")
    {
        return OpCode::MUL;
    }
    if (s == "DIV")
    {
        return OpCode::DIV;
    }
    if (s == "MOD")
    {
        return OpCode::MOD;
    }
    if (s == "BIT_AND")
    {
        return OpCode::BIT_AND;
    }
    if (s == "BIT_OR")
    {
        return OpCode::BIT_OR;
    }
    if (s == "BIT_XOR")
    {
        return OpCode::BIT_XOR;
    }
    if (s == "SHL")
    {
        return OpCode::SHL;
    }
    if (s == "SHR")
    {
        return OpCode::SHR;
    }
    if (s == "LOG_AND")
    {
        return OpCode::LOG_AND;
    }
    if (s == "LOG_OR")
    {
        return OpCode::LOG_OR;
    }
    if (s == "EQUAL")
    {
        return OpCode::EQUAL;
    }
    if (s == "NOT_EQUAL")
    {
        return OpCode::NOT_EQUAL;
    }
    if (s == "GREATER_THAN")
    {
        return OpCode::GREATER_THAN;
    }
    if (s == "LESS_THAN")
    {
        return OpCode::LESS_THAN;
    }
    if (s == "GREATER_EQUAL")
    {
        return OpCode::GREATER_EQUAL;
    }
    if (s == "LESS_EQUAL")
    {
        return OpCode::LESS_EQUAL;
    }
    if (s == "POINTER_ADD")
    {
        return OpCode::POINTER_ADD;
    }
    if (s == "POINTER_SUB")
    {
        return OpCode::POINTER_SUB;
    }
    if (s == "MIN")
    {
        return OpCode::MIN;
    }
    if (s == "MAX")
    {
        return OpCode::MAX;
    }
    if (s == "ROTATE_LEFT")
    {
        return OpCode::ROTATE_LEFT;
    }
    if (s == "ROTATE_RIGHT")
    {
        return OpCode::ROTATE_RIGHT;
    }
    if (s == "CAST")
    {
        return OpCode::CAST;
    }

    return OpCode::NONE;
}

/**
 * Convert a string representation of Scope to the corresponding enum value.
 *
 * @param s Scope string (e.g. "GLOBAL", "FUNCTION").
 *
 * @return Equivalent Scope enum value.
 */
static constexpr Scope parseScope(const std::string &s) noexcept
{
    if (s == "GLOBAL")
    {
        return Scope::GLOBAL;
    }
    if (s == "STATIC")
    {
        return Scope::STATIC;
    }
    if (s == "FUNCTION")
    {
        return Scope::FUNCTION;
    }

    return Scope::GLOBAL;
}

/**
 * Convert a string representation of StorageDuration to the corresponding enum value.
 *
 * @param s StorageDuration string (e.g. "AUTO", "STATIC").
 *
 * @return Equivalent StorageDuration enum value.
 */
static constexpr StorageDuration parseStorageDuration(const std::string &s) noexcept
{
    if (s == "AUTO")
    {
        return StorageDuration::AUTO;
    }
    if (s == "STATIC")
    {
        return StorageDuration::STATIC;
    }
    if (s == "EXTERN")
    {
        return StorageDuration::EXTERN;
    }
    if (s == "REGISTER")
    {
        return StorageDuration::REGISTER;
    }
    if (s == "THREAD_LOCAL")
    {
        return StorageDuration::THREAD_LOCAL;
    }

    return StorageDuration::AUTO;
}

/**
 * Convert a string representation of Linkage to the corresponding enum value.
 *
 * @param s Linkage string (e.g. "INTERNAL", "EXTERNAL").
 *
 * @return Equivalent Linkage enum value.
 */
static constexpr Linkage parseLinkage(const std::string &s) noexcept
{
    if (s == "INTERNAL")
    {
        return Linkage::INTERNAL;
    }
    if (s == "EXTERNAL")
    {
        return Linkage::EXTERNAL;
    }

    return Linkage::NONE;
}

/**
 * Convert a string representation of AccessorKind to the corresponding enum value.
 *
 * @param s AccessorKind string (e.g. "DEREF", "FIELD").
 *
 * @return Equivalent AccessorKind enum value.
 */
static constexpr AccessorKind parseAccessorKind(const std::string &s) noexcept
{
    if (s == "DEREF")
    {
        return AccessorKind::DEREF;
    }
    if (s == "ARRAY")
    {
        return AccessorKind::ARRAY;
    }
    if (s == "FIELD")
    {
        return AccessorKind::FIELD;
    }
    if (s == "ADDRESS_OF")
    {
        return AccessorKind::ADDRESS_OF;
    }
    if (s == "OFFSET")
    {
        return AccessorKind::OFFSET;
    }
    if (s == "BIT_SLICE")
    {
        return AccessorKind::BIT_SLICE;
    }

    return AccessorKind::DEREF;
}

// forward declarations
static Operand parseOperand(const json &j);
static Initializer parseInitializer(const json &j);

/**
 * Parse an Accessor from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Accessor&).
 *
 * @return Populated Accessor.
 */
static Accessor parseAccessor(const json &j)
{
    Accessor acc;
    acc.kind = parseAccessorKind(j.at("kind").get<std::string>());

    switch (acc.kind)
    {
    case AccessorKind::DEREF:
        acc.data = DerefAccessor{};
        break;

    case AccessorKind::ARRAY:
        acc.data = ArrayAccessor{parseOperand(j.at("index"))};
        break;

    case AccessorKind::FIELD: {
        FieldAccessor fa;
        j.at("field_id").get_to(fa.field_id);
        acc.data = std::move(fa);
        break;
    }

    case AccessorKind::ADDRESS_OF: {
        AddressOfAccessor aoa;
        j.at("target_type_id").get_to(aoa.target_type_id);
        acc.data = std::move(aoa);
        break;
    }

    case AccessorKind::OFFSET: {
        OffsetAccessor oa;
        oa.offset = parseOperand(j.at("offset"));
        acc.data = std::move(oa);
        break;
    }

    case AccessorKind::BIT_SLICE: {
        BitSliceAccessor bsa;
        bsa.bit_start = j.at("bit_start").get<unsigned>();
        bsa.bit_size = j.at("bit_size").get<unsigned>();
        acc.data = std::move(bsa);
        break;
    }
    }

    return acc;
}

/**
 * Parse an Operand from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Operand&).
 *
 * @return Populated Operand (constant or variable).
 */
static Operand parseOperand(const json &j)
{
    const std::string type = j.at("type").get<std::string>();

    if (type == "constant")
    {
        ConstantOperand co;
        j.at("type_id").get_to(co.type_id);
        co.value = j.at("value").get<std::string>();
        return co;
    }
    else // "variable"
    {
        VariableOperand vo;
        j.at("variable_id").get_to(vo.id);
        for (const auto &acc_j : j.at("access_path"))
        {
            vo.access_path.push_back(parseAccessor(acc_j));
        }
        if (const auto it = j.find("result_type_id"); it != j.end() && !it->is_null())
        {
            TypeId result_type_id;
            it->get_to(result_type_id);
            vo.result_type_id = result_type_id;
        }

        return vo;
    }
}

/**
 * Parse an Initializer from its JSON representation.
 *
 * @param j JSON value (array for an InitializerList, object for a scalar Operand).
 *
 * @return Populated Initializer.
 */
static Initializer parseInitializer(const json &j)
{
    if (j.is_array())
    {
        auto list = std::make_shared<InitializerList>();
        for (const auto &elem : j)
        {
            list->elements.push_back(parseInitializer(elem));
        }

        return list;
    }
    else
    {
        return parseOperand(j);
    }
}

/**
 * Parse a Type from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Type&).
 *
 * @return Populated Type.
 */
static Type parseType(const json &j)
{
    Type t;
    j.at("id").get_to(t.id);
    t.name = j.at("name").get<std::string>();
    t.kind = parseTypeKind(j.at("kind").get<std::string>());
    t.size_bits = j.at("size_bits").get<size_t>();
    t.alignment = j.at("alignment").get<size_t>();
    t.is_const = j.at("is_const").get<bool>();
    t.is_volatile = j.at("is_volatile").get<bool>();
    t.is_atomic = j.at("is_atomic").get<bool>();

    switch (t.kind)
    {
    case TypeKind::UNKNOWN:
        t.data = UnknownType{};
        break;

    case TypeKind::VOID:
        t.data = VoidType{};
        break;

    case TypeKind::ENUM:
        t.data = EnumType{};
        break;

    case TypeKind::BOOL:
        t.data = BoolType{};
        break;

    case TypeKind::REAL:
        t.data = FloatType{};
        break;

    case TypeKind::INTEGER: {
        IntegerType it;
        it.is_unsigned = j.at("is_unsigned").get<bool>();
        t.data = it;
        break;
    }

    case TypeKind::POINTER: {
        PointerType pt;
        j.at("pointee_type_id").get_to(pt.pointee_type_id);
        pt.is_restrict = j.at("is_restrict").get<bool>();
        t.data = pt;
        break;
    }

    case TypeKind::ARRAY: {
        ArrayType at;
        j.at("element_type_id").get_to(at.element_type_id);
        if (j.contains("element_count"))
        {
            at.element_count = j.at("element_count").get<size_t>();
        }
        t.data = at;
        break;
    }

    case TypeKind::STRUCT: {
        StructType st;
        for (const auto &fid : j.at("fields"))
        {
            VariableId vid;
            fid.get_to(vid);
            st.fields.push_back(vid);
        }
        t.data = std::move(st);
        break;
    }

    case TypeKind::UNION: {
        UnionType ut;
        for (const auto &fid : j.at("fields"))
        {
            VariableId vid;
            fid.get_to(vid);
            ut.fields.push_back(vid);
        }
        t.data = std::move(ut);
        break;
    }

    case TypeKind::FUNCTION: {
        FunctionType ft;
        j.at("return_type_id").get_to(ft.return_type_id);
        for (const auto &ptid : j.at("parameter_type_ids"))
        {
            TypeId tid;
            ptid.get_to(tid);
            ft.parameter_type_ids.push_back(tid);
        }
        ft.is_variadic = j.at("is_variadic").get<bool>();
        t.data = std::move(ft);
        break;
    }

    case TypeKind::COMPLEX: {
        ComplexType ct;
        j.at("component_type_id").get_to(ct.component_type_id);
        t.data = ct;
        break;
    }
    }

    return t;
}

/**
 * Parse a Variable from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Variable&).
 *
 * @return Populated Variable.
 */
static Variable parseVariable(const json &j)
{
    Variable v;
    j.at("id").get_to(v.id);
    v.name = j.at("name").get<std::string>();
    j.at("type_id").get_to(v.type_id);
    v.source_location = parseSourceLocation(j.at("location"));
    v.artificial = j.at("artificial").get<bool>();

    if (j.contains("scope"))
    {
        StandardVariable sv;
        sv.scope = parseScope(j.at("scope").get<std::string>());
        sv.storage_duration = parseStorageDuration(j.at("storage_duration").get<std::string>());
        sv.linkage = parseLinkage(j.at("linkage").get<std::string>());
        if (j.contains("initial_value"))
        {
            sv.initial_value = parseInitializer(j.at("initial_value"));
        }
        v.data = std::move(sv);
    }
    else
    {
        FieldVariable fv;
        fv.byte_offset = j.at("byte_offset").get<size_t>();
        if (j.contains("bitfield_size"))
        {
            fv.bitfield_size = j.at("bitfield_size").get<size_t>();
        }
        if (j.contains("bitfield_offset"))
        {
            fv.bitfield_offset = j.at("bitfield_offset").get<size_t>();
        }
        v.data = std::move(fv);
    }

    return v;
}

/**
 * Parse a SwitchCase from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const SwitchCase&).
 *
 * @return Populated SwitchCase.
 */
static SwitchCase parseSwitchCase(const json &j)
{
    SwitchCase sc;
    j.at("target_block_id").get_to(sc.target_block_id);
    if (j.contains("low_value"))
    {
        sc.low_value = parseOperand(j.at("low_value"));
    }
    if (j.contains("high_value"))
    {
        sc.high_value = parseOperand(j.at("high_value"));
    }

    return sc;
}

/**
 * Parse a PhiIncomingValue from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const PhiIncomingValue&).
 *
 * @return Populated PhiIncomingValue.
 */
static PhiIncomingValue parsePhiIncomingValue(const json &j)
{
    PhiIncomingValue piv;
    j.at("block_id").get_to(piv.block_id);
    piv.value = parseOperand(j.at("value"));
    return piv;
}

/**
 * Parse an Instruction from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Instruction&).
 *
 * @return Populated Instruction with kind-specific data variant.
 */
static Instruction parseInstruction(const json &j)
{
    Instruction instr;
    j.at("id").get_to(instr.id);
    j.at("parent_block_id").get_to(instr.parent_block_id);
    instr.kind = parseInstructionKind(j.at("kind").get<std::string>());
    instr.is_terminator = j.at("is_terminator").get<bool>();
    instr.source_location = parseSourceLocation(j.at("location"));

    switch (instr.kind)
    {
    case InstructionKind::ASSIGN: {
        AssignInstruction ai;
        ai.opcode = parseOpCode(j.at("opcode").get<std::string>());
        ai.lhs = parseOperand(j.at("lhs"));
        if (j.contains("rhs1"))
            ai.rhs1 = parseOperand(j.at("rhs1"));
        if (j.contains("rhs2"))
            ai.rhs2 = parseOperand(j.at("rhs2"));
        if (j.contains("rhs3"))
            ai.rhs3 = parseOperand(j.at("rhs3"));
        instr.data = std::move(ai);
        break;
    }

    case InstructionKind::CALL: {
        CallInstruction ci;
        if (j.contains("lhs"))
            ci.lhs = parseOperand(j.at("lhs"));
        ci.callee = parseOperand(j.at("callee"));
        for (const auto &arg : j.at("arguments"))
        {
            ci.arguments.push_back(parseOperand(arg));
        }
        instr.data = std::move(ci);
        break;
    }

    case InstructionKind::RETURN: {
        ReturnInstruction ri;
        if (j.contains("return_value"))
            ri.return_value = parseOperand(j.at("return_value"));
        instr.data = std::move(ri);
        break;
    }

    case InstructionKind::COND: {
        CondInstruction ci;
        ci.opcode = parseOpCode(j.at("opcode").get<std::string>());
        ci.lhs = parseOperand(j.at("lhs"));
        ci.rhs = parseOperand(j.at("rhs"));
        j.at("true_target").get_to(ci.true_target);
        j.at("false_target").get_to(ci.false_target);
        instr.data = std::move(ci);
        break;
    }

    case InstructionKind::SWITCH: {
        SwitchInstruction si;
        si.index = parseOperand(j.at("index"));
        for (const auto &sc : j.at("cases"))
        {
            si.cases.push_back(parseSwitchCase(sc));
        }
        instr.data = std::move(si);
        break;
    }

    case InstructionKind::GOTO: {
        GotoInstruction gi;
        j.at("target").get_to(gi.target);
        instr.data = gi;
        break;
    }

    case InstructionKind::LABEL: {
        LabelInstruction li;
        li.label = parseOperand(j.at("label"));
        instr.data = std::move(li);
        break;
    }

    case InstructionKind::ASM: {
        AsmInstruction ai;
        ai.assembly_string = j.at("assembly_string").get<std::string>();
        ai.is_volatile = j.at("is_volatile").get<bool>();
        for (const auto &out : j.at("outputs"))
        {
            AsmOperandConstraint aoc;
            aoc.constraint = out.at("constraint").get<std::string>();
            aoc.operand = parseOperand(out.at("operand"));
            ai.outputs.push_back(std::move(aoc));
        }
        for (const auto &in : j.at("inputs"))
        {
            AsmOperandConstraint aoc;
            aoc.constraint = in.at("constraint").get<std::string>();
            aoc.operand = parseOperand(in.at("operand"));
            ai.inputs.push_back(std::move(aoc));
        }
        for (const auto &clob : j.at("clobbers"))
        {
            ai.clobbers.push_back(clob.get<std::string>());
        }
        instr.data = std::move(ai);
        break;
    }

    case InstructionKind::PHI: {
        PhiInstruction pi;
        pi.lhs = parseOperand(j.at("lhs"));
        for (const auto &iv : j.at("incoming_values"))
        {
            pi.incoming_values.push_back(parsePhiIncomingValue(iv));
        }
        instr.data = std::move(pi);
        break;
    }

    case InstructionKind::CLOBBER: {
        ClobberInstruction ci;
        ci.clobbered_variable = parseOperand(j.at("clobbered_variable"));
        instr.data = std::move(ci);
        break;
    }

    case InstructionKind::UNREACHABLE:
        instr.data = UnreachableInstruction{};
        break;

    case InstructionKind::ABORT:
        instr.data = AbortInstruction{};
        break;

    case InstructionKind::NOP:
        instr.data = std::monostate{};
        break;

    case InstructionKind::UNKNOWN:
    default: {
        UnknownInstruction ui;
        ui.description = j.value("description", "");
        instr.data = std::move(ui);
        break;
    }
    }

    return instr;
}

/**
 * Parse a Function from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Function&).
 *
 * @return Populated Function.
 */
static Function parseFunction(const json &j)
{
    Function f;
    j.at("id").get_to(f.id);
    f.name = j.at("name").get<std::string>();
    j.at("return_type_id").get_to(f.return_type_id);
    if (j.contains("source_location"))
        f.source_location = parseSourceLocation(j.at("source_location"));
    for (const auto &pid : j.at("parameter_ids"))
    {
        VariableId vid;
        pid.get_to(vid);
        f.parameter_ids.push_back(vid);
    }
    for (const auto &lvid : j.at("local_variable_ids"))
    {
        VariableId vid;
        lvid.get_to(vid);
        f.local_variable_ids.push_back(vid);
    }
    for (const auto &bid : j.at("block_ids"))
    {
        BlockId bk;
        bid.get_to(bk);
        f.block_ids.push_back(bk);
    }

    return f;
}

/**
 * Parse a Block from its JSON representation.
 *
 * @param j JSON object produced by to_json(json&, const Block&).
 *
 * @return Populated Block.
 */
static Block parseBlock(const json &j)
{
    Block b;
    j.at("id").get_to(b.id);
    j.at("parent").get_to(b.parent);
    b.name = j.at("name").get<std::string>();
    for (const auto &pred : j.at("predecessors"))
    {
        BlockId bid;
        pred.get_to(bid);
        b.predecessors.push_back(bid);
    }
    for (const auto &succ : j.at("successors"))
    {
        BlockId bid;
        succ.get_to(bid);
        b.successors.push_back(bid);
    }
    for (const auto &iid : j.at("instruction_ids"))
    {
        InstructionId iid_;
        iid.get_to(iid_);
        b.instruction_ids.push_back(iid_);
    }

    return b;
}

} // namespace Core

namespace Exporters
{

Core::CodeModel JSONImporter::importFromFile(const std::string &filepath)
{
    std::ifstream ifs(filepath);
    if (!ifs.is_open())
    {
        throw std::runtime_error("JSONImporter: cannot open file: " + filepath);
    }

    return importFromStream(ifs);
}

Core::CodeModel JSONImporter::importFromStream(std::istream &is)
{
    json j;
    is >> j;

    Core::CodeModel model;

    for (const auto &jt : j.at("types"))
    {
        Core::Type *tp = model.createType();
        *tp = Core::parseType(jt);
    }

    for (const auto &jv : j.at("variables"))
    {
        Core::Variable *vp = model.createVariable();
        *vp = Core::parseVariable(jv);
    }

    for (const auto &jf : j.at("functions"))
    {
        Core::Function *fp = model.createFunction();
        *fp = Core::parseFunction(jf);
    }

    for (const auto &jb : j.at("blocks"))
    {
        Core::Block *bp = model.createBlock();
        *bp = Core::parseBlock(jb);
    }

    for (const auto &ji : j.at("instructions"))
    {
        Core::Instruction *ip = model.createInstruction();
        *ip = Core::parseInstruction(ji);
    }

    return model;
}

} // namespace Exporters

} // namespace CodeListener
