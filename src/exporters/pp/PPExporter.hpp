/**
 * @file PPExporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the pretty-printer that renders the CodeModel as three-address code.
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

#include <fstream>
#include <string>
#include <vector>

#include "Exporter.hpp"
#include "InstructionId.hpp"
#include "Operand.hpp"

namespace CodeListener::Exporters
{

/**
 * Exports the CodeModel in the 3AC "pp" text format produced by the old cl_pp.cc listener.
 * This is the primary replacement for libcl.a's "pp" listener.
 *
 * Switch instructions are unfolded to if-else chains by consuming the
 * SwitchToIf annotation (via the inherited analysis_manager), so the
 * same normalized form is available to any other consumer.
 */
class PPExporter : public Exporter
{
  public:
    /**
     * Constructs an exporter that writes to an already opened stream.
     *
     * @param os Output stream receiving pretty-printed code.
     */
    explicit PPExporter(std::ostream &os);

    /**
     * Constructs an exporter that writes to a file.
     *
     * @param filepath Output file path opened by the exporter.
     */
    explicit PPExporter(const std::string &filepath);

  protected:
    /** @copydoc Core::CodeModelVisitor::onBeginFunction */
    void onBeginFunction(const Core::CodeModel &model, const Core::Function &func) override;

    /** @copydoc Core::CodeModelVisitor::onEndFunction */
    void onEndFunction(const Core::CodeModel &model, const Core::Function &func) override;

    /** @copydoc Core::CodeModelVisitor::shouldVisitBlock */
    bool shouldVisitBlock(const Core::CodeModel &model, const Core::Block &block) override;

    /** @copydoc Core::CodeModelVisitor::onBeginBlock */
    void onBeginBlock(const Core::CodeModel &model, const Core::Block &block) override;

    /** @copydoc Core::CodeModelVisitor::onVisitInstruction */
    void onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &inst) override;

    /** @copydoc Core::CodeModelVisitor::onEndModel */
    void onEndModel(const Core::CodeModel &model) override;

  private:
    /** Owned file stream used when the exporter was constructed from a path. */
    std::ofstream file_os;

    /** Effective output stream used regardless of construction mode. */
    std::ostream &os;

    /** Tracks whether the current block already emitted an explicit terminator. */
    bool block_has_terminator = false;

    /** Render an arbitrary operand in the legacy pp syntax. */
    std::string fmtOperand(const Core::Operand &op, const Core::CodeModel &model) const;

    /** Render a variable reference together with its accessors in the legacy pp syntax. */
    std::string fmtVar(Core::VariableId id, const std::vector<Core::Accessor> &accessors,
                       const Core::CodeModel &model) const;

    /** Render a constant operand in the legacy pp syntax. */
    std::string fmtConst(const Core::ConstantOperand &cst, const Core::CodeModel &model) const;

    /** Convert a binary opcode to the corresponding textual operator. */
    static std::string binopSym(Core::OpCode op);

    /** Check whether an opcode should be rendered as a binary operation. */
    static bool isBinOp(Core::OpCode op);

    /** Emit one switch instruction using the normalized if/else lowering. */
    void emitSwitchUnfolded(const Core::SwitchInstruction &sw, Core::InstructionId inst_id,
                            const Core::CodeModel &model);

    /** @copydoc Core::CodeModelVisitor::onEndBlock */
    void onEndBlock(const Core::CodeModel &model, const Core::Block &block) override;
};

} // namespace CodeListener::Exporters
