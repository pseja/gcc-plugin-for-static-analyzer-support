/**
 * @file AnalysisContext.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the service bundle exposed to native analyzers during execution.
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

#ifndef ANALYSIS_CONTEXT_HPP
#define ANALYSIS_CONTEXT_HPP

#ifdef __cplusplus

#include <string>

#include "AnalysisManager.hpp"
#include "DiagnosticReporter.hpp"
#include "DOTVerbosity.hpp"

namespace CodeListener
{

namespace Core
{
class CodeModel;
}

/**
 * Bundles all services an external analyzer might need to analyze the CodeModel.
 */
class AnalysisContext
{
  public:
    /** Diagnostic sink used to report findings and internal failures. */
    Core::DiagnosticReporter &reporter;

    /** Shared cache of lazily computed annotations derived from the current model. */
    AnnotationServices::AnalysisManager &analysis_manager;

    /**
     * Constructs a service bundle over the caller-provided reporter and annotation cache.
     *
     * @param reporter Diagnostic sink exposed to analyzers.
     * @param analysis_manager Shared annotation cache exposed to analyzers.
     */
    AnalysisContext(Core::DiagnosticReporter &reporter, AnnotationServices::AnalysisManager &analysis_manager)
        : reporter(reporter), analysis_manager(analysis_manager)
    {
    }

    /** Non-copyable because the context stores references to external services. */
    AnalysisContext(const AnalysisContext &) = delete;
    /** Non-assignable because the context stores references to external services. */
    AnalysisContext &operator=(const AnalysisContext &) = delete;

    /**
     * Exports the model as a DOT CFG graph.
     *
     * @param model Model to serialize.
     * @param path Destination file path.
     * @param verbosity Requested DOT output verbosity.
     */
    void exportDot(const Core::CodeModel &model, const std::string &path,
                   Exporters::DotVerbosity verbosity = Exporters::DotVerbosity::CLEAN);

    /**
     * Exports the model as a pretty-printed three-address-code listing.
     *
     * @param model Model to serialize.
     * @param path Destination file path.
     */
    void exportPP(const Core::CodeModel &model, const std::string &path);

    /**
     * Serializes the model to the internal JSON format.
     *
     * @param model Model to serialize.
     * @param path Destination file path.
     */
    void exportJson(const Core::CodeModel &model, const std::string &path);
};

} // namespace CodeListener

#endif /* __cplusplus */
#endif /* ANALYSIS_CONTEXT_HPP */
