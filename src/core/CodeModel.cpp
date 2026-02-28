#include <ranges>

#include "CodeModel.hpp"

namespace CodeListener
{

namespace Core
{

const Type *CodeModel::getType(NodeId id) const
{
    auto it = types_map.find(id);
    return it != types_map.end() ? &it->second : nullptr;
}
const Variable *CodeModel::getVariable(NodeId id) const
{
    auto it = variables_map.find(id);
    return it != variables_map.end() ? &it->second : nullptr;
}
const Function *CodeModel::getFunction(NodeId id) const
{
    auto it = functions_map.find(id);
    return it != functions_map.end() ? &it->second : nullptr;
}
const Block *CodeModel::getBlock(NodeId id) const
{
    auto it = blocks_map.find(id);
    return it != blocks_map.end() ? &it->second : nullptr;
}
const Instruction *CodeModel::getInstruction(NodeId id) const
{
    auto it = instructions_map.find(id);
    return it != instructions_map.end() ? &it->second : nullptr;
}

const std::unordered_map<NodeId, Type> &CodeModel::getTypes() const
{
    return types_map;
}
const std::unordered_map<NodeId, Variable> &CodeModel::getVariables() const
{
    return variables_map;
}
const std::unordered_map<NodeId, Function> &CodeModel::getFunctions() const
{
    return functions_map;
}
const std::unordered_map<NodeId, Block> &CodeModel::getBlocks() const
{
    return blocks_map;
}
const std::unordered_map<NodeId, Instruction> &CodeModel::getInstructions() const
{
    return instructions_map;
}

void CodeModel::addType(Type type)
{
    types_map[type.id] = std::move(type);
}
void CodeModel::addVariable(Variable var)
{
    variables_map[var.id] = std::move(var);
}
void CodeModel::addFunction(Function func)
{
    functions_map[func.id] = std::move(func);
}
void CodeModel::addBlock(Block block)
{
    blocks_map[block.id] = std::move(block);
}
void CodeModel::addInstruction(Instruction instr)
{
    instructions_map[instr.id] = std::move(instr);
}

template <typename T>
void CodeModel::attachAnnotation(NodeId target_id, const std::string &key, std::unique_ptr<T> data)
{
    annotations[target_id][key] = std::move(data);
}

template <typename T> const T *CodeModel::getAnnotation(NodeId target_id, const std::string &key) const
{
    auto it = annotations.find(target_id);
    if (it != annotations.end())
    {
        auto it2 = it->second.find(key);
        if (it2 != it->second.end())
        {
            return dynamic_cast<const T *>(it2->second.get());
        }
    }
    return nullptr;
}

auto CodeModel::types() const
{
    return types_map | std::views::values;
}
auto CodeModel::variables() const
{
    return variables_map | std::views::values;
}
auto CodeModel::functions() const
{
    return functions_map | std::views::values;
}
auto CodeModel::blocks() const
{
    return blocks_map | std::views::values;
}
auto CodeModel::instructions() const
{
    return instructions_map | std::views::values;
}

auto CodeModel::parametersOf(const Function &func) const
{
    return func.parameter_ids |
           std::views::transform([this](NodeId id) -> const Variable & { return variables_map.at(id); });
}
auto CodeModel::blocksOf(const Function &func) const
{
    return func.block_ids | std::views::transform([this](NodeId id) -> const Block & { return blocks_map.at(id); });
}
auto CodeModel::instructionsOf(const Block &block) const
{
    return block.instruction_ids |
           std::views::transform([this](NodeId id) -> const Instruction & { return instructions_map.at(id); });
}
auto CodeModel::instructionsOf(const Function &func) const
{
    return blocksOf(func) | std::views::transform([this](const Block &b) { return instructionsOf(b); }) |
           std::views::join;
}

} // namespace Core

} // namespace CodeListener
