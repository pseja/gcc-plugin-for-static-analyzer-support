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

/** GCC pass wrapper that forwards each visited function body to the adapter. */
class Pass : public gimple_opt_pass
{
  public:
    /**
     * Constructs the custom GCC pass.
     *
     * @param ctx GCC pass-manager context.
     * @param adapter Adapter invoked for each processed function.
     */
    Pass(gcc::context *ctx, GCCAdapter &adapter);

    /**
     * Execute the pass for one GCC function.
     *
     * @param fun GCC function being optimized/analyzed.
     *
     * @return Zero to indicate no changes were made.
     */
    unsigned int execute(function *fun) override final;

    /**
     * Clone the pass instance for GCC's internal pass management.
     *
     * @return Pointer to a newly allocated clone of this pass.
     */
    opt_pass *clone() override final;

  private:
    /** Adapter translating processed functions into the CodeModel. */
    GCCAdapter &adapter;

    /** Static GCC metadata describing this pass to the pass manager. */
    static const struct pass_data pass_metadata;
};

} // namespace CodeListener::CompilerAbstractionLayer
