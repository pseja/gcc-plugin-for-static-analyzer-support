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

    void walk(const CodeModel &model);

  protected:
    virtual void onBeginModel(const CodeModel &);
    virtual void onEndModel(const CodeModel &);

    virtual void onVisitType(const CodeModel &, const Type &);
    virtual void onVisitVariable(const CodeModel &, const Variable &);

    virtual void onBeginFunction(const CodeModel &, const Function &);
    virtual void onEndFunction(const CodeModel &, const Function &);

    virtual bool shouldVisitBlock(const CodeModel &, const Block &);

    virtual void onBeginBlock(const CodeModel &, const Block &);
    virtual void onEndBlock(const CodeModel &, const Block &);

    virtual void onBeforeInstruction(const CodeModel &, const Instruction &);
    virtual void onVisitInstruction(const CodeModel &, const Instruction &);
    virtual void onAfterInstruction(const CodeModel &, const Instruction &);
};

} // namespace CodeListener::Core
