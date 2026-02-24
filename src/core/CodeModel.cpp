#include "CodeModel.hpp"

namespace CodeListener
{

namespace Core
{

const Type *CodeModel::getType(NodeId id) const
{
    auto it = types.find(id);
    return it != types.end() ? &it->second : nullptr;
}
const Variable *CodeModel::getVariable(NodeId id) const
{
    auto it = variables.find(id);
    return it != variables.end() ? &it->second : nullptr;
}
const Function *CodeModel::getFunction(NodeId id) const
{
    auto it = functions.find(id);
    return it != functions.end() ? &it->second : nullptr;
}
const Block *CodeModel::getBlock(NodeId id) const
{
    auto it = blocks.find(id);
    return it != blocks.end() ? &it->second : nullptr;
}
const Instruction *CodeModel::getInstruction(NodeId id) const
{
    auto it = instructions.find(id);
    return it != instructions.end() ? &it->second : nullptr;
}

const std::unordered_map<NodeId, Type> &CodeModel::getTypes() const
{
    return types;
}
const std::unordered_map<NodeId, Variable> &CodeModel::getVariables() const
{
    return variables;
}
const std::unordered_map<NodeId, Function> &CodeModel::getFunctions() const
{
    return functions;
}
const std::unordered_map<NodeId, Block> &CodeModel::getBlocks() const
{
    return blocks;
}
const std::unordered_map<NodeId, Instruction> &CodeModel::getInstructions() const
{
    return instructions;
}

void CodeModel::addType(Type type)
{
    types[type.id] = std::move(type);
}
void CodeModel::addVariable(Variable var)
{
    variables[var.id] = std::move(var);
}
void CodeModel::addFunction(Function func)
{
    functions[func.id] = std::move(func);
}
void CodeModel::addBlock(Block block)
{
    blocks[block.id] = std::move(block);
}
void CodeModel::addInstruction(Instruction instr)
{
    instructions[instr.id] = std::move(instr);
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

} // namespace Core

} // namespace CodeListener
