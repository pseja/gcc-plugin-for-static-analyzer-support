#pragma once

#include <string>
#include <vector>

#include "AsmOperandConstraint.hpp"

namespace CodeListener::Core
{

struct AsmInstruction
{
    std::string assembly_string;
    std::vector<AsmOperandConstraint> inputs;
    std::vector<AsmOperandConstraint> outputs;
    std::vector<std::string> clobbers;
    bool is_volatile{false};
};

} // namespace CodeListener::Core
