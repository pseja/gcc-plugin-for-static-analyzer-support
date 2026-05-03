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
    /** @name Query System API */
    ///@{

    /**
     * Look up a type by its stable identifier.
     *
     * @param id Identifier of the requested type.
     *
     * @return Pointer to the stored type or `nullptr` when the id is invalid.
     */
    const Type *getType(TypeId id) const;

    /**
     * Look up a variable by its stable identifier.
     *
     * @param id Identifier of the requested variable.
     *
     * @return Pointer to the stored variable or `nullptr` when the id is invalid.
     */
    const Variable *getVariable(VariableId id) const;

    /**
     * Look up a function by its stable identifier.
     *
     * @param id Identifier of the requested function.
     *
     * @return Pointer to the stored function or `nullptr` when the id is invalid.
     */
    const Function *getFunction(FunctionId id) const;

    /**
     * Look up a basic block by its stable identifier.
     *
     * @param id Identifier of the requested block.
     *
     * @return Pointer to the stored block or `nullptr` when the id is invalid.
     */
    const Block *getBlock(BlockId id) const;

    /**
     * Look up an instruction by its stable identifier.
     *
     * @param id Identifier of the requested instruction.
     *
     * @return Pointer to the stored instruction or `nullptr` when the id is invalid.
     */
    const Instruction *getInstruction(InstructionId id) const;

    /**
     * Look up a mutable type by its stable identifier.
     *
     * @param id Identifier of the requested type.
     *
     * @return Pointer to the stored type or `nullptr` when the id is invalid.
     */
    Type *getTypeMutable(TypeId id);

    /**
     * Look up a mutable variable by its stable identifier.
     *
     * @param id Identifier of the requested variable.
     *
     * @return Pointer to the stored variable or `nullptr` when the id is invalid.
     */
    Variable *getVariableMutable(VariableId id);

    /**
     * Look up a mutable function by its stable identifier.
     *
     * @param id Identifier of the requested function.
     *
     * @return Pointer to the stored function or `nullptr` when the id is invalid.
     */
    Function *getFunctionMutable(FunctionId id);

    /**
     * Look up a mutable block by its stable identifier.
     *
     * @param id Identifier of the requested block.
     *
     * @return Pointer to the stored block or `nullptr` when the id is invalid.
     */
    Block *getBlockMutable(BlockId id);

    /**
     * Look up a mutable instruction by its stable identifier.
     *
     * @param id Identifier of the requested instruction.
     *
     * @return Pointer to the stored instruction or `nullptr` when the id is invalid.
     */
    Instruction *getInstructionMutable(InstructionId id);

    /**
     * Return the source filename associated with the model.
     *
     * @return Source filename recorded for the current translation unit.
     */
    const std::string &getFilename() const;

    /**
     * Set the source filename associated with the model.
     *
     * @param filename Source filename of the current translation unit.
     */
    void setFilename(const std::string &filename);

    ///@}

    /** @name Range Views */
    ///@{

    /** @return Flat view over all stored types. */
    [[nodiscard]] auto types() const
    {
        return std::views::all(types_pool);
    }

    /** @return Flat view over all stored variables. */
    [[nodiscard]] auto variables() const
    {
        return std::views::all(variables_pool);
    }

    /** @return Flat view over all stored functions. */
    [[nodiscard]] auto functions() const
    {
        return std::views::all(functions_pool);
    }

    /** @return Flat view over all stored basic blocks. */
    [[nodiscard]] auto blocks() const
    {
        return std::views::all(blocks_pool);
    }

    /** @return Flat view over all stored instructions. */
    [[nodiscard]] auto instructions() const
    {
        return std::views::all(instructions_pool);
    }

    /**
     * Enumerate the parameters owned by a function.
     *
     * @param func Function whose parameter list should be traversed.
     *
     * @return Lazy view over the function parameters.
     */
    [[nodiscard]] auto parametersOf(const Function &func) const
    {
        return func.parameter_ids |
               std::views::transform([this](VariableId id) -> const Variable & { return variables_pool[id.index]; });
    }

    /**
     * Enumerate the blocks owned by a function.
     *
     * @param func Function whose blocks should be traversed.
     *
     * @return Lazy view over the function blocks.
     */
    [[nodiscard]] auto blocksOf(const Function &func) const
    {
        return func.block_ids |
               std::views::transform([this](BlockId id) -> const Block & { return blocks_pool[id.index]; });
    }

    /**
     * Enumerate the instructions owned by a block.
     *
     * @param block Block whose instructions should be traversed.
     *
     * @return Lazy view over the block instructions.
     */
    [[nodiscard]] auto instructionsOf(const Block &block) const
    {
        return block.instruction_ids | std::views::transform([this](InstructionId id) -> const Instruction & {
                   return instructions_pool[id.index];
               });
    }

    /**
     * Enumerate all instructions contained in a function.
     *
     * @param func Function whose instructions should be traversed.
     *
     * @return Lazy flattened view over instructions in all owned blocks.
     */
    [[nodiscard]] auto instructionsOf(const Function &func) const
    {
        return blocksOf(func) | std::views::transform([this](const Block &b) { return instructionsOf(b); }) |
               std::views::join;
    }

    ///@}

    /** @name Model Builder API */
    ///@{

    /**
     * Append a default-initialized type to the model.
     *
     * @return Pointer to the newly created type.
     */
    Type *createType();

    /**
     * Append a default-initialized variable to the model.
     *
     * @return Pointer to the newly created variable.
     */
    Variable *createVariable();

    /**
     * Append a default-initialized function to the model.
     *
     * @return Pointer to the newly created function.
     */
    Function *createFunction();

    /**
     * Append a default-initialized block to the model.
     *
     * @return Pointer to the newly created block.
     */
    Block *createBlock();

    /**
     * Append a default-initialized instruction to the model.
     *
     * @return Pointer to the newly created instruction.
     */
    Instruction *createInstruction();

    /**
     * Record an externally constructed type in the model storage.
     *
     * @param type Pointer to the type object to append.
     */
    void addType(Type *type);

    /**
     * Record an externally constructed variable in the model storage.
     *
     * @param variable Pointer to the variable object to append.
     */
    void addVariable(Variable *variable);

    /**
     * Record an externally constructed function in the model storage.
     *
     * @param function Pointer to the function object to append.
     */
    void addFunction(Function *function);

    /**
     * Record an externally constructed block in the model storage.
     *
     * @param block Pointer to the block object to append.
     */
    void addBlock(Block *block);

    /**
     * Record an externally constructed instruction in the model storage.
     *
     * @param instruction Pointer to the instruction object to append.
     */
    void addInstruction(Instruction *instruction);

    ///@}

  private:
    /** Stable storage for all types in insertion order. */
    std::deque<Type> types_pool;

    /** Stable storage for all variables in insertion order. */
    std::deque<Variable> variables_pool;

    /** Stable storage for all functions in insertion order. */
    std::deque<Function> functions_pool;

    /** Stable storage for all blocks in insertion order. */
    std::deque<Block> blocks_pool;

    /** Stable storage for all instructions in insertion order. */
    std::deque<Instruction> instructions_pool;

    /** Source filename associated with the translation unit represented by the model. */
    std::string filename;
};

} // namespace CodeListener::Core
