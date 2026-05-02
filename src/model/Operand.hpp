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

struct Accessor;

struct ConstantOperand
{
    TypeId type_id;
    std::string value;
};

struct VariableOperand
{
    VariableId id;
    std::vector<Accessor> access_path;
    std::optional<TypeId> result_type_id; // type of the full expression, if known
};

using Operand = std::variant<ConstantOperand, VariableOperand>;

struct DerefAccessor
{
    // *ptr
};

struct ArrayAccessor
{
    // ptr[index] or arr[index]
    Operand index;
};

struct FieldAccessor
{
    // obj.field or ptr->field
    VariableId field_id;
};

struct AddressOfAccessor
{
    // &expr
    TypeId target_type_id;
};

struct OffsetAccessor
{
    // (char*)ptr + offset (pointer arithmetic)
    Operand offset;
};

struct BitSliceAccessor
{
    // expr[start:end] (bit field extraction)
    unsigned bit_start;
    unsigned bit_size;
};

using AccessorData =
    std::variant<DerefAccessor, ArrayAccessor, FieldAccessor, AddressOfAccessor, OffsetAccessor, BitSliceAccessor>;

struct Accessor
{
    // FIXME: not needed anymore thanks to the AccessorData variant, but keeping it for easier querying and debugging
    // for now
    AccessorKind kind;
    AccessorData data;
};

} // namespace CodeListener::Core
