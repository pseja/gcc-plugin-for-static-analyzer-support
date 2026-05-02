/**
 * @file Pass.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the custom GCC pass that forwards functions to the GCC adapter.
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

#include "Pass.hpp"
#include "DiagnosticReporter.hpp"
#include "GCCAdapter.hpp"
#include "PluginContext.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

Pass::Pass(gcc::context *ctx, GCCAdapter &adapter) : gimple_opt_pass(pass_metadata, ctx), adapter(adapter)
{
}

unsigned int Pass::execute(function *fun)
{
    PluginContext::getInstance().getDiagnosticReporter().report(
        Core::DiagnosticLevel::Debug, std::string("Processing function: ") + function_name(fun));

    adapter.processFunction(fun);

    return 0;
}

opt_pass *Pass::clone()
{
    return new Pass(*this);
}

const struct pass_data Pass::pass_metadata = {
    .type = GIMPLE_PASS,
    .name = "codelistener_pass",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = PROP_cfg,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0,
};

} // namespace CodeListener::CompilerAbstractionLayer
