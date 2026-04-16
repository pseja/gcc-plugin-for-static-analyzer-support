#pragma once

#include <unordered_map>

#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "InstructionId.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Exporters
{

// maps per-TU IDs to the corresponding merged-model IDs
struct IdMaps
{
    std::unordered_map<Core::TypeId, Core::TypeId> type_map;
    std::unordered_map<Core::VariableId, Core::VariableId> var_map;
    std::unordered_map<Core::FunctionId, Core::FunctionId> func_map;
    std::unordered_map<Core::BlockId, Core::BlockId> block_map;
    std::unordered_map<Core::InstructionId, Core::InstructionId> instr_map;
};

} // namespace CodeListener::Exporters
