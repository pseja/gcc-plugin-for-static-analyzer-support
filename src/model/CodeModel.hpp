/**
 * @file CodeModel.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the central storage and query interface for the CodeModel.
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

#pragma once

#include <deque>  // std::deque
#include <ranges> // std::views::transform, std::views::join

// #include "AnnotationBase.hpp"
#include "Block.hpp"
#include "BlockId.hpp"
#include "Function.hpp"
#include "FunctionId.hpp"
#include "Instruction.hpp"
#include "InstructionId.hpp"
#include "Type.hpp"
#include "TypeId.hpp"
#include "Variable.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

/**
 * Owns the compiler-independent intermediate representation.
 */
class CodeModel
{
  public:
    // --- Query System API ---
    const Type *getType(TypeId id) const;
    const Variable *getVariable(VariableId id) const;
    const Function *getFunction(FunctionId id) const;
    const Block *getBlock(BlockId id) const;
    const Instruction *getInstruction(InstructionId id) const;

    Type *getTypeMutable(TypeId id);
    Variable *getVariableMutable(VariableId id);
    Function *getFunctionMutable(FunctionId id);
    Block *getBlockMutable(BlockId id);
    Instruction *getInstructionMutable(InstructionId id);

    const std::string &getFilename() const;
    void setFilename(const std::string &filename);

    // flat range views
    [[nodiscard]] auto types() const
    {
        return std::views::all(types_pool);
    }
    [[nodiscard]] auto variables() const
    {
        return std::views::all(variables_pool);
    }
    [[nodiscard]] auto functions() const
    {
        return std::views::all(functions_pool);
    }
    [[nodiscard]] auto blocks() const
    {
        return std::views::all(blocks_pool);
    }
    [[nodiscard]] auto instructions() const
    {
        return std::views::all(instructions_pool);
    }
    [[nodiscard]] auto parametersOf(const Function &func) const
    {
        return func.parameter_ids |
               std::views::transform([this](VariableId id) -> const Variable & { return variables_pool[id.index]; });
    }
    [[nodiscard]] auto blocksOf(const Function &func) const
    {
        return func.block_ids |
               std::views::transform([this](BlockId id) -> const Block & { return blocks_pool[id.index]; });
    }
    [[nodiscard]] auto instructionsOf(const Block &block) const
    {
        return block.instruction_ids | std::views::transform([this](InstructionId id) -> const Instruction & {
                   return instructions_pool[id.index];
               });
    }
    [[nodiscard]] auto instructionsOf(const Function &func) const
    {
        return blocksOf(func) | std::views::transform([this](const Block &b) { return instructionsOf(b); }) |
               std::views::join;
    }

    // --- Model Builder API ---
    Type *createType();
    Variable *createVariable();
    Function *createFunction();
    Block *createBlock();
    Instruction *createInstruction();

    void addType(Type *type);
    void addVariable(Variable *variable);
    void addFunction(Function *function);
    void addBlock(Block *block);
    void addInstruction(Instruction *instruction);

  private:
    // flat storage
    std::deque<Type> types_pool;
    std::deque<Variable> variables_pool;
    std::deque<Function> functions_pool;
    std::deque<Block> blocks_pool;
    std::deque<Instruction> instructions_pool;

    std::string filename;
};

} // namespace CodeListener::Core
