/**
 * @file CodeModelVisitor.cpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Implements visitor traversal over the CodeModel.
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

#include "CodeModelVisitor.hpp"

#include "Block.hpp"
#include "CodeModel.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "Variable.hpp"

namespace CodeListener::Core
{

void CodeModelVisitor::walk(const CodeModel &model)
{
    onBeginModel(model);

    for (const auto &type : model.types())
    {
        onVisitType(model, type);
    }

    for (const auto &var : model.variables())
    {
        onVisitVariable(model, var);
    }

    for (const auto &func : model.functions())
    {
        onBeginFunction(model, func);

        for (const auto block_id : func.block_ids)
        {
            const Block &block = *model.getBlock(block_id);
            if (!shouldVisitBlock(model, block))
            {
                continue;
            }

            onBeginBlock(model, block);

            for (const auto instr_id : block.instruction_ids)
            {
                const Instruction &instr = *model.getInstruction(instr_id);
                onBeforeInstruction(model, instr);
                onVisitInstruction(model, instr);
                onAfterInstruction(model, instr);
            }

            onEndBlock(model, block);
        }

        onEndFunction(model, func);
    }

    onEndModel(model);
}

void CodeModelVisitor::onBeginModel(const CodeModel &)
{
}
void CodeModelVisitor::onEndModel(const CodeModel &)
{
}

void CodeModelVisitor::onVisitType(const CodeModel &, const Type &)
{
}
void CodeModelVisitor::onVisitVariable(const CodeModel &, const Variable &)
{
}

void CodeModelVisitor::onBeginFunction(const CodeModel &, const Function &)
{
}
void CodeModelVisitor::onEndFunction(const CodeModel &, const Function &)
{
}

bool CodeModelVisitor::shouldVisitBlock(const CodeModel &, const Block &)
{
    return true;
}

void CodeModelVisitor::onBeginBlock(const CodeModel &, const Block &)
{
}
void CodeModelVisitor::onEndBlock(const CodeModel &, const Block &)
{
}

void CodeModelVisitor::onBeforeInstruction(const CodeModel &, const Instruction &)
{
}
void CodeModelVisitor::onVisitInstruction(const CodeModel &, const Instruction &)
{
}
void CodeModelVisitor::onAfterInstruction(const CodeModel &, const Instruction &)
{
}

} // namespace CodeListener::Core
