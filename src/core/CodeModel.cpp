#include "CodeModel.hpp"
#include "BlockId.hpp"
#include "FunctionId.hpp"
#include "Instruction.hpp"
#include "InstructionId.hpp"
#include "TypeId.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

const Type *CodeModel::getType(TypeId id) const
{
    if (!id.isValid() || id.index >= types_pool.size())
    {
        return nullptr;
    }

    return &types_pool[id];
}
const Variable *CodeModel::getVariable(VariableId id) const
{
    if (!id.isValid() || id.index >= variables_pool.size())
    {
        return nullptr;
    }

    return &variables_pool[id];
}
const Function *CodeModel::getFunction(FunctionId id) const
{
    if (!id.isValid() || id.index >= functions_pool.size())
    {
        return nullptr;
    }

    return &functions_pool[id];
}
const Block *CodeModel::getBlock(BlockId id) const
{
    if (!id.isValid() || id.index >= blocks_pool.size())
    {
        return nullptr;
    }

    return &blocks_pool[id];
}
const Instruction *CodeModel::getInstruction(InstructionId id) const
{
    if (!id.isValid() || id.index >= instructions_pool.size())
    {
        return nullptr;
    }

    return &instructions_pool[id];
}

Type *CodeModel::getTypeMutable(TypeId id)
{
    if (!id.isValid() || id.index >= types_pool.size())
    {
        return nullptr;
    }

    return &types_pool[id];
}
Variable *CodeModel::getVariableMutable(VariableId id)
{
    if (!id.isValid() || id.index >= variables_pool.size())
    {
        return nullptr;
    }

    return &variables_pool[id];
}
Function *CodeModel::getFunctionMutable(FunctionId id)
{
    if (!id.isValid() || id.index >= functions_pool.size())
    {
        return nullptr;
    }

    return &functions_pool[id];
}
Block *CodeModel::getBlockMutable(BlockId id)
{
    if (!id.isValid() || id.index >= blocks_pool.size())
    {
        return nullptr;
    }

    return &blocks_pool[id];
}
Instruction *CodeModel::getInstructionMutable(InstructionId id)
{
    if (!id.isValid() || id.index >= instructions_pool.size())
    {
        return nullptr;
    }

    return &instructions_pool[id];
}

const std::deque<Type> &CodeModel::getTypes() const
{
    return types_pool;
}
const std::deque<Variable> &CodeModel::getVariables() const
{
    return variables_pool;
}
const std::deque<Function> &CodeModel::getFunctions() const
{
    return functions_pool;
}
const std::deque<Block> &CodeModel::getBlocks() const
{
    return blocks_pool;
}
const std::deque<Instruction> &CodeModel::getInstructions() const
{
    return instructions_pool;
}

const std::string &CodeModel::getFilename() const
{
    return filename;
}
void CodeModel::setFilename(const std::string &name)
{
    filename = name;
}

Type *CodeModel::createType()
{
    Type type;
    type.id = TypeId{types_pool.size()};
    types_pool.push_back(std::move(type));
    return &types_pool.back();
}
Variable *CodeModel::createVariable()
{
    Variable variable;
    variable.id = VariableId{variables_pool.size()};
    variables_pool.push_back(std::move(variable));
    return &variables_pool.back();
}
Function *CodeModel::createFunction()
{
    Function function;
    function.id = FunctionId{functions_pool.size()};
    functions_pool.push_back(std::move(function));
    return &functions_pool.back();
}
Block *CodeModel::createBlock()
{
    Block block;
    block.id = BlockId{blocks_pool.size()};
    blocks_pool.push_back(std::move(block));
    return &blocks_pool.back();
}
Instruction *CodeModel::createInstruction()
{
    Instruction instruction;
    instruction.id = InstructionId{instructions_pool.size()};
    instructions_pool.push_back(std::move(instruction));
    return &instructions_pool.back();
}

void CodeModel::addType(Type *type)
{
    type->id = TypeId{types_pool.size()};
    types_pool.push_back(*type);
}
void CodeModel::addVariable(Variable *variable)
{
    variable->id = VariableId{variables_pool.size()};
    variables_pool.push_back(*variable);
}
void CodeModel::addFunction(Function *function)
{
    function->id = FunctionId{functions_pool.size()};
    functions_pool.push_back(*function);
}
void CodeModel::addBlock(Block *block)
{
    block->id = BlockId{blocks_pool.size()};
    blocks_pool.push_back(*block);
}
void CodeModel::addInstruction(Instruction *instruction)
{
    instruction->id = InstructionId{instructions_pool.size()};
    instructions_pool.push_back(*instruction);
}

// template <typename T>
// void CodeModel::attachAnnotation(NodeId target_id, const std::string &key, std::unique_ptr<T> data)
// {
//     annotations[target_id][key] = std::move(data);
// }

// template <typename T>
// const T *CodeModel::getAnnotation(NodeId target_id, const std::string &key) const
// {
//     auto it = annotations.find(target_id);
//     if (it != annotations.end())
//     {
//         auto it2 = it->second.find(key);
//         if (it2 != it->second.end())
//         {
//             return dynamic_cast<const T *>(it2->second.get());
//         }
//     }
//     return nullptr;
// }

// auto CodeModel::types() const
// {
//     return types_pool | std::views::values;
// }
// auto CodeModel::variables() const
// {
//     return variables_pool | std::views::values;
// }
// auto CodeModel::functions() const
// {
//     return functions_pool | std::views::values;
// }
// auto CodeModel::blocks() const
// {
//     return blocks_pool | std::views::values;
// }
// auto CodeModel::instructions() const
// {
//     return instructions_pool | std::views::values;
// }

// auto CodeModel::parametersOf(const Function &func) const
// {
//     return func.parameter_ids |
//            std::views::transform([this](NodeId id) -> const Variable & { return variables_pool.at(id); });
// }
// auto CodeModel::blocksOf(const Function &func) const
// {
//     return func.block_ids | std::views::transform([this](NodeId id) -> const Block & { return blocks_pool.at(id); });
// }
// auto CodeModel::instructionsOf(const Block &block) const
// {
//     return block.instruction_ids |
//            std::views::transform([this](NodeId id) -> const Instruction & { return instructions_pool.at(id); });
// }
// auto CodeModel::instructionsOf(const Function &func) const
// {
//     return blocksOf(func) | std::views::transform([this](const Block &b) { return instructionsOf(b); }) |
//            std::views::join;
// }

} // namespace CodeListener::Core
