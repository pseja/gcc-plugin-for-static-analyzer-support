/**
 * @file register_plugin.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the GCC plugin entry-point helpers.
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

#include <gcc-plugin.h> // for plugin_name_args

namespace CodeListener::CompilerAbstractionLayer
{

/**
 * Print plugin and compiler version information in the same format as the legacy plugin.
 *
 * @param plugin_info GCC plugin metadata supplied to `plugin_init`.
 * @param version GCC version structure describing the host compiler.
 */
void print_info(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version);

} // namespace CodeListener::CompilerAbstractionLayer
