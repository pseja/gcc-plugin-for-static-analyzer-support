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
    /**
     * Constructs an exporter that writes to an already opened stream.
     *
     * @param os Output stream receiving DOT text.
     * @param verbosity Requested output verbosity.
     */
    explicit DOTExporter(std::ostream &os, DotVerbosity verbosity = DotVerbosity::CLEAN);

    /**
     * Constructs an exporter that writes to a file.
     *
     * @param filepath Output file path opened by the exporter.
     * @param verbosity Requested output verbosity.
     */
    explicit DOTExporter(const std::string &filepath, DotVerbosity verbosity = DotVerbosity::CLEAN);

  protected:
    /** @copydoc Core::CodeModelVisitor::shouldVisitBlock */
    bool shouldVisitBlock(const Core::CodeModel &model, const Core::Block &block) override;

    /** @copydoc Core::CodeModelVisitor::onBeginModel */
    void onBeginModel(const Core::CodeModel &model) override;

    /** @copydoc Core::CodeModelVisitor::onEndModel */
    void onEndModel(const Core::CodeModel &model) override;

    /** @copydoc Core::CodeModelVisitor::onBeginFunction */
    void onBeginFunction(const Core::CodeModel &model, const Core::Function &func) override;

    /** @copydoc Core::CodeModelVisitor::onEndFunction */
    void onEndFunction(const Core::CodeModel &model, const Core::Function &func) override;

    /** @copydoc Core::CodeModelVisitor::onBeginBlock */
    void onBeginBlock(const Core::CodeModel &model, const Core::Block &block) override;

    /** @copydoc Core::CodeModelVisitor::onEndBlock */
    void onEndBlock(const Core::CodeModel &model, const Core::Block &block) override;

    /** @copydoc Core::CodeModelVisitor::onVisitInstruction */
    void onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &instr) override;

  private:
    /** Owned file stream used when the exporter was constructed from a path. */
    std::ofstream file_os;

    /** Effective output stream used regardless of construction mode. */
    std::ostream &os;

    /** Selected DOT verbosity level. */
    DotVerbosity verbosity;

    /** Deferred edge output emitted after block bodies were rendered. */
    std::ostringstream edge_buffer;

    /** File currently being emitted as a DOT cluster. */
    std::string curr_file;

    /** Monotonic identifier used when opening file clusters. */
    int file_cluster_id{0};

    /** Close the currently open file cluster, if any. */
    void closeFileCluster();

    /** Open a cluster for the supplied file name. */
    void openFileCluster(const std::string &file);

    /** Emit CFG edges leaving the supplied block. */
    void emitBlockEdges(const Core::CodeModel &model, const Core::Block &block);

    /** Check whether a block should be visible in the current verbosity mode. */
    bool isBlockVisible(const Core::CodeModel &model, Core::BlockId id) const;

    /** Format the source endpoint string of an outgoing edge. */
    std::string edgeSrcNodeStr(const Core::CodeModel &model, const Core::Block &block) const;

    /** Format the target endpoint string of an outgoing edge. */
    std::string edgeTargetNodeStr(Core::BlockId id) const;

    /** Classify terminal instructions for the compact DOT rendering. */
    static std::tuple<const char *, const char *, const char *> cleanTerminalInfo(const Core::Instruction &instr);

    /** Render one instruction in the verbosity-specific DOT label format. */
    std::string exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr);

    /** Render one operand in DOT text form. */
    std::string formatOperand(const Core::CodeModel &model, const Core::Operand &op);

    /** Render one accessor chain fragment in DOT text form. */
    std::string formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc, const std::string &base);

    /** Render one instruction as plain label text. */
    std::string formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr);

    /** Escape text so it is safe inside DOT string literals. */
    std::string escape(const std::string &str);

    /** Convert an operation code to its textual operator spelling. */
    std::string opCodeToString(Core::OpCode opcode);
};

} // namespace CodeListener::Exporters
