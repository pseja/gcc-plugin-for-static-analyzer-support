/**
 * @file FieldVariable.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares variable payloads representing structure or union fields.
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

namespace CodeListener::Core
{

/** Represents a struct/union field variable with layout information. */
struct FieldVariable
{
    /** Byte offset of this field within its containing type, if known. */
    std::optional<unsigned> byte_offset;
    /** Size of the bitfield in bits; absent for non-bitfield members. */
    std::optional<unsigned> bitfield_size;   // in bits
    /** Bit offset within the storage unit; absent for non-bitfield members. */
    std::optional<unsigned> bitfield_offset; // in bits from the start of the storage unit
};

} // namespace CodeListener::Core
