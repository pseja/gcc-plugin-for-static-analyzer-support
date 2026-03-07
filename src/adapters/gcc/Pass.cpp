#include "Pass.hpp"
#include "CodeModel.hpp"
#include "GCCAdapter.hpp"
#include "PluginContext.hpp"

namespace CodeListener::CompilerAbstractionLayer
{

Pass::Pass(gcc::context *ctx) : opt_pass(pass_metadata, ctx)
{
}

unsigned int Pass::execute(function *fun)
{
    PluginContext::getInstance().getDiagnosticReporter().report(
        Core::DiagnosticLevel::Debug, std::string("Processing function: ") + function_name(fun));

    Core::CodeModel &my_model = PluginContext::getInstance().getCodeModel();

    Core::GCCAdapter adapter(my_model);

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
