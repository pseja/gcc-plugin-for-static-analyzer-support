#pragma once

#include <gcc-plugin.h> // for plugin_name_args

namespace CodeListener::CompilerAbstractionLayer
{

void print_info(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version);

} // namespace CodeListener::CompilerAbstractionLayer
