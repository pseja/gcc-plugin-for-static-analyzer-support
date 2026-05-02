/**
 * @file Pass.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the custom GCC pass that forwards functions to the GCC adapter.
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

#include <gcc-plugin.h>
#include <function.h>  // function
#include <tree-pass.h> // gimple_opt_pass, gcc::context, pass_data

#include "GCCAdapter.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

// TODO: rename to something more specific
class Pass : public gimple_opt_pass
{
  public:
    Pass(gcc::context *ctx, GCCAdapter &adapter);

    unsigned int execute(function *fun) override final;

    opt_pass *clone() override final;

  private:
    GCCAdapter &adapter;
    static const struct pass_data pass_metadata;
};

} // namespace CodeListener::CompilerAbstractionLayer
