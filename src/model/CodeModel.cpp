/**
 * @file CodeModel.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements the central storage and query interface for the CodeModel.
 * @date 2026-05-02
 *
 * @copyright Copyright (c) 2026 Lukáš Pšeja
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

} // namespace CodeListener::Core
