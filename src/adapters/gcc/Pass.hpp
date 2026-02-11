#pragma once

#include <gcc-plugin.h>
#include <function.h> // function
#include <tree-pass.h> // opt_pass, gcc::context, pass_data

namespace CodeListener
{

namespace GCC_ADAPTER
{

// TODO: rename to something more specific
class Pass : public opt_pass
{
  public:
    Pass(gcc::context *ctx);

    unsigned int execute(function *fun) override final;

    opt_pass *clone() override final;

  private:
    static const struct pass_data pass_metadata;
};

} // namespace GCC_ADAPTER

} // namespace CodeListener
