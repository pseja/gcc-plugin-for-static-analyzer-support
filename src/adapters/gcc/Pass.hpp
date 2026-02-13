#pragma once

#include <gcc-plugin.h>
#include <function.h> // function
#include <tree-pass.h> // opt_pass, gcc::context, pass_data

namespace CodeListener
{

namespace CompilerAbstractionLayer
{

// TODO: rename to something more specific
class Pass : public opt_pass
{
  public:
    Pass(gcc::context *ctx);

    unsigned int execute(function *fun) override final;

    // FIXME: maybe not needed
    opt_pass *clone() override final;

  private:
    static const struct pass_data pass_metadata;
};

} // namespace CompilerAbstractionLayer

} // namespace CodeListener
