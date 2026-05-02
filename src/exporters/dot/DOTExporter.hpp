/**
 * @file DOTExporter.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the Graphviz DOT exporter for CodeModel graphs.
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
#include <iostream>
#include <sstream>
#include <string>

#include "DOTVerbosity.hpp"
#include "Exporter.hpp"

namespace CodeListener::Exporters
{

/**
 * Exports a CodeModel as a Graphviz DOT control-flow graph.
 */
class DOTExporter : public Exporter
{
  public:
    explicit DOTExporter(std::ostream &os, DotVerbosity verbosity = DotVerbosity::CLEAN);
    explicit DOTExporter(const std::string &filepath, DotVerbosity verbosity = DotVerbosity::CLEAN);

  protected:
    bool shouldVisitBlock(const Core::CodeModel &model, const Core::Block &block) override;

    void onBeginModel(const Core::CodeModel &model) override;
    void onEndModel(const Core::CodeModel &model) override;

    void onBeginFunction(const Core::CodeModel &model, const Core::Function &func) override;
    void onEndFunction(const Core::CodeModel &model, const Core::Function &func) override;

    void onBeginBlock(const Core::CodeModel &model, const Core::Block &block) override;
    void onEndBlock(const Core::CodeModel &model, const Core::Block &block) override;

    void onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &instr) override;

  private:
    std::ofstream file_os;
    std::ostream &os;
    DotVerbosity verbosity;

    std::ostringstream edge_buffer;

    std::string curr_file;
    int file_cluster_id{0};

    void closeFileCluster();
    void openFileCluster(const std::string &file);

    void emitBlockEdges(const Core::CodeModel &model, const Core::Block &block);

    bool isBlockVisible(const Core::CodeModel &model, Core::BlockId id) const;
    std::string edgeSrcNodeStr(const Core::CodeModel &model, const Core::Block &block) const;
    std::string edgeTargetNodeStr(Core::BlockId id) const;
    static std::tuple<const char *, const char *, const char *> cleanTerminalInfo(const Core::Instruction &instr);

    std::string exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr);
    std::string formatOperand(const Core::CodeModel &model, const Core::Operand &op);
    std::string formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc, const std::string &base);
    std::string formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr);

    std::string escape(const std::string &str);
    std::string opCodeToString(Core::OpCode opcode);
};

} // namespace CodeListener::Exporters
