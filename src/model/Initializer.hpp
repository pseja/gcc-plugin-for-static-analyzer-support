/**
 * @file Initializer.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares initializer variants stored in variable definitions.
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

#include <memory>  // std::shared_ptr
#include <variant> // std::variant
#include <vector>  // std::vector

#include "Operand.hpp"

namespace CodeListener::Core
{

/** Forward declaration of a nested initializer list. */
struct InitializerList;

/** Discriminated union covering scalar and aggregate initializers. */
using Initializer = std::variant<Operand, std::shared_ptr<InitializerList>>;

/** Represents an aggregate initializer containing nested element initializers. */
struct InitializerList
{
    /** Ordered element initializers contained in the aggregate. */
    std::vector<Initializer> elements;
};

} // namespace CodeListener::Core
