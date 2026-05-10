/**
 * @file CodeModelVisitor.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares visitor traversal over the CodeModel.
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

#include "Block.hpp"
#include "CodeModel.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "Variable.hpp"

namespace CodeListener::Core
{

/**
 * Provides a structured traversal over all top-level entities, blocks, and instructions in a CodeModel.
 */
class CodeModelVisitor
{
  public:
    virtual ~CodeModelVisitor() = default;

    /**
     * Traverses every entity in @p model, calling the virtual hooks in order.
     *
     * @param model CodeModel to traverse.
     */
    void walk(const CodeModel &model);

  protected:
    /**
     * Called once before the traversal begins.
     *
     * @param model CodeModel being traversed.
     */
    virtual void onBeginModel(const CodeModel &model);

    /**
     * Called once after the traversal ends.
     *
     * @param model CodeModel being traversed.
     */
    virtual void onEndModel(const CodeModel &model);

    /**
     * Called for each type in the model.
     *
     * @param model CodeModel being traversed.
     * @param type  Type being visited.
     */
    virtual void onVisitType(const CodeModel &model, const Type &type);

    /**
     * Called for each variable in the model.
     *
     * @param model CodeModel being traversed.
     * @param var   Variable being visited.
     */
    virtual void onVisitVariable(const CodeModel &model, const Variable &var);

    /**
     * Called before the blocks of a function are visited.
     *
     * @param model CodeModel being traversed.
     * @param func  Function about to be entered.
     */
    virtual void onBeginFunction(const CodeModel &model, const Function &func);

    /**
     * Called after all blocks of a function have been visited.
     *
     * @param model CodeModel being traversed.
     * @param func  Function that was just finished.
     */
    virtual void onEndFunction(const CodeModel &model, const Function &func);

    /**
     * Controls whether a block should be visited.
     *
     * @param model CodeModel being traversed.
     * @param block Block candidate.
     * @return true if the block and its instructions should be visited; false to skip.
     */
    virtual bool shouldVisitBlock(const CodeModel &model, const Block &block);

    /**
     * Called before the instructions of a block are visited.
     *
     * @param model CodeModel being traversed.
     * @param block Block about to be entered.
     */
    virtual void onBeginBlock(const CodeModel &model, const Block &block);

    /**
     * Called after all instructions of a block have been visited.
     *
     * @param model CodeModel being traversed.
     * @param block Block that was just finished.
     */
    virtual void onEndBlock(const CodeModel &model, const Block &block);

    /**
     * Called for each instruction in the block.
     *
     * @param model CodeModel being traversed.
     * @param instr Instruction being visited.
     */
    virtual void onVisitInstruction(const CodeModel &model, const Instruction &instr);

    /**
     * Called immediately after each instruction has been visited.
     *
     * @param model CodeModel being traversed.
     * @param instr Instruction that was just visited.
     */
    virtual void onAfterInstruction(const CodeModel &model, const Instruction &instr);

    /**
     * Called immediately before each instruction is visited.
     *
     * @param model CodeModel being traversed.
     * @param instr Instruction about to be visited.
     */
    virtual void onBeforeInstruction(const CodeModel &model, const Instruction &instr);
};

} // namespace CodeListener::Core
