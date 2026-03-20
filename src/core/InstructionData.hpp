#pragma once

#include <variant>

#include "AbortInstruction.hpp"
#include "AsmInstruction.hpp"
#include "AssignInstruction.hpp"
#include "CallInstruction.hpp"
#include "ClobberInstruction.hpp"
#include "CondInstruction.hpp"
#include "GotoInstruction.hpp"
#include "LabelInstruction.hpp"
#include "PhiInstruction.hpp"
#include "ReturnInstruction.hpp"
#include "SwitchInstruction.hpp"
#include "UnknownInstruction.hpp"
#include "UnreachableInstruction.hpp"

namespace CodeListener::Core
{

using InstructionData =
    std::variant<std::monostate, AssignInstruction, CallInstruction, GotoInstruction, LabelInstruction, AsmInstruction,
                 ReturnInstruction, CondInstruction, SwitchInstruction, PhiInstruction, ClobberInstruction,
                 UnreachableInstruction, AbortInstruction, UnknownInstruction>;

} // namespace CodeListener::Core
