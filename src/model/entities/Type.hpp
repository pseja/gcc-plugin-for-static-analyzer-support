/**
 * @file Type.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares type entities stored in the CodeModel.
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

#include <string>

#include "TypeId.hpp"
#include "TypeKind.hpp"
#include "TypeData.hpp"

namespace CodeListener::Core
{

/** Represents one canonicalized type stored in the CodeModel. */
struct Type
{
    /** Stable identifier of the type. */
    TypeId id;

    /** Human-readable type name such as `int` or `struct Foo`. */
    std::string name;

    /** Whether the type is qualified with `const`. */
    bool is_const{false};

    /** Whether the type is qualified with `volatile`. */
    bool is_volatile{false};
    // bool is_restrict{false}; // only for pointers

    /** Whether the type is qualified with `_Atomic`. */
    bool is_atomic{false};

    /** Size of the type in bits, primarily used for bit-fields. */
    int size_bits;
    // int size_bytes; // sizeof

    /** Alignment requirement of the type in bytes. */
    int alignment; // in bytes

    /** Cached coarse-grained kind used for quick checks and debugging. */
    TypeKind kind;

    /** Detailed payload describing the concrete type shape. */
    TypeData data;
};

} // namespace CodeListener::Core
