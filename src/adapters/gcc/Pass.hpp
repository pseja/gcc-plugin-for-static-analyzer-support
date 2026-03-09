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
