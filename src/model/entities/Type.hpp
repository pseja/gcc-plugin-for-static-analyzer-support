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

struct Type
{
    TypeId id;
    std::string name; // "int", "struct Foo", ...

    // qualifiers and attributes
    bool is_const{false};
    bool is_volatile{false};
    // bool is_restrict{false}; // only for pointers
    bool is_atomic{false}; // _Atomic

    // size and memory layout
    int size_bits; // for bitfields
    // int size_bytes; // sizeof
    int alignment; // in bytes

    // FIXME: not needed anymore thanks to the TypeData variant, but keeping it for easier querying and debugging
    // for now
    TypeKind kind;
    TypeData data;
};

} // namespace CodeListener::Core
