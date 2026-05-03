/**
 * @file Operand.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares operand variants used by CodeModel instructions.
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

#include <optional>
#include <variant>
#include <vector>

#include "AccessorKind.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

/** Forward declaration of one accessor step applied to a variable operand. */
struct Accessor;

/** Represents a literal constant value used by an instruction. */
struct ConstantOperand
{
    /** Identifier of the constant's static type. */
    TypeId type_id;

    /** Constant text as emitted by the frontend. */
    std::string value;
};

/** Represents a variable reference together with any dereference/field/index access path. */
struct VariableOperand
{
    /** Identifier of the referenced root variable. */
    VariableId id;

    /** Ordered accessor chain applied to the root variable. */
    std::vector<Accessor> access_path;

    /** Optional type of the full expression after all accessors were applied. */
    std::optional<TypeId> result_type_id; // type of the full expression, if known
};

/** Discriminated union covering all operand categories supported by the CodeModel. */
using Operand = std::variant<ConstantOperand, VariableOperand>;

/** Accessor step representing pointer dereference, for example `*ptr`. */
struct DerefAccessor
{
};

/** Accessor step representing array or pointer indexing, for example `arr[index]` or `ptr[index]`. */
struct ArrayAccessor
{
    /** Operand describing the accessed index expression. */
    Operand index;
};

/** Accessor step representing structure or union field access, for example `obj.field` or `ptr->field`. */
struct FieldAccessor
{
    /** Identifier of the accessed field variable. */
    VariableId field_id;
};

/** Accessor step representing address-of on a subexpression, for example `&expr`. */
struct AddressOfAccessor
{
    /** Identifier of the resulting pointer type. */
    TypeId target_type_id;
};

/** Accessor step representing pointer arithmetic by a byte or element offset, for example `(char*)ptr + offset`. */
struct OffsetAccessor
{
    /** Operand describing the applied offset expression. */
    Operand offset;
};

/** Accessor step representing extraction of a bit slice or bit-field, for example `expr[start:end]`. */
struct BitSliceAccessor
{
    /** Starting bit offset inside the accessed value. */
    unsigned bit_start;

    /** Width of the extracted slice in bits. */
    unsigned bit_size;
};

/** Discriminated union covering all accessor categories supported by variable operands. */
using AccessorData =
    std::variant<DerefAccessor, ArrayAccessor, FieldAccessor, AddressOfAccessor, OffsetAccessor, BitSliceAccessor>;

/** Represents one step in a variable operand access path. */
struct Accessor
{
    /** Cached accessor kind used for quick checks and debugging. */
    AccessorKind kind;

    /** Detailed payload describing the concrete accessor step. */
    AccessorData data;
};

} // namespace CodeListener::Core
