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
    explicit PPExporter(std::ostream &os);
    explicit PPExporter(const std::string &filepath);

  protected:
    void onBeginFunction(const Core::CodeModel &model, const Core::Function &func) override;
    void onEndFunction(const Core::CodeModel &model, const Core::Function &func) override;

    bool shouldVisitBlock(const Core::CodeModel &model, const Core::Block &block) override;
    void onBeginBlock(const Core::CodeModel &model, const Core::Block &block) override;

    void onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &inst) override;

    void onEndModel(const Core::CodeModel &model) override;

  private:
    std::ofstream file_os;
    std::ostream &os;

    // tracks whether the current block already has an explicit terminator
    bool block_has_terminator = false;

    // operand formatting
    std::string fmtOperand(const Core::Operand &op, const Core::CodeModel &model) const;
    std::string fmtVar(Core::VariableId id, const std::vector<Core::Accessor> &accessors,
                       const Core::CodeModel &model) const;
    std::string fmtConst(const Core::ConstantOperand &cst, const Core::CodeModel &model) const;

    // instruction helpers
    static std::string binopSym(Core::OpCode op);
    static bool isBinOp(Core::OpCode op);

    // emit switch unfolded as a sequence of equality checks + if-else jumps
    void emitSwitchUnfolded(const Core::SwitchInstruction &sw, Core::InstructionId inst_id,
                            const Core::CodeModel &model);

    void onEndBlock(const Core::CodeModel &model, const Core::Block &block) override;
};

} // namespace CodeListener::Exporters
