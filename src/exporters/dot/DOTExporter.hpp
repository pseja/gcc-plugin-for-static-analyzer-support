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

    /**
     * Open a cluster for the supplied file name.
     *
     * @param file Source file name to label the cluster.
     */
    void openFileCluster(const std::string &file);

    /**
     * Emit a block in CLEAN verbosity.
     *
     * @param model CodeModel being exported.
     * @param block Block to render.
     */
    void emitCleanBlock(const Core::CodeModel &model, const Core::Block &block);

    /**
     * Emit a block in COMPACT or FULL verbosity.
     *
     * @param model CodeModel being exported.
     * @param block Block to render.
     */
    void emitDetailedBlock(const Core::CodeModel &model, const Core::Block &block);

    /**
     * Emit one inter-procedural call edge according to the active verbosity.
     *
     * @param model        CodeModel being exported.
     * @param instr        Call instruction that triggers the edge.
     * @param callee_id    Identifier of the called function.
     * @param target_block First visible block in the callee.
     */
    void emitCallGraphEdge(const Core::CodeModel &model, const Core::Instruction &instr, Core::FunctionId callee_id,
                           const Core::Block &target_block);

    /**
     * Emit CFG edges leaving the supplied block.
     *
     * @param model CodeModel being exported.
     * @param block Source block whose outgoing edges are emitted.
     */
    void emitBlockEdges(const Core::CodeModel &model, const Core::Block &block);

    /**
     * Check whether a block should be visible in the current verbosity mode.
     *
     * @param model CodeModel being exported.
     * @param id    Identifier of the block to test.
     * @return true if the block should appear in the DOT output; false otherwise.
     */
    bool isBlockVisible(const Core::CodeModel &model, Core::BlockId id) const;
};

} // namespace CodeListener::Exporters
