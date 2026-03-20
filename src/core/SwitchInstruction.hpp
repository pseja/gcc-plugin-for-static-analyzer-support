#pragma once

#include <vector>

#include "Operand.hpp"
#include "SwitchCase.hpp"

namespace CodeListener::Core
{

struct SwitchInstruction
{
    Operand index;
    std::vector<SwitchCase> cases;
};

} // namespace CodeListener::Core
