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
    Core::DiagnosticReporter &reporter;
    AnnotationServices::AnalysisManager &analysis_manager;

    AnalysisContext(Core::DiagnosticReporter &reporter, AnnotationServices::AnalysisManager &analysis_manager)
        : reporter(reporter), analysis_manager(analysis_manager)
    {
    }

    AnalysisContext(const AnalysisContext &) = delete;
    AnalysisContext &operator=(const AnalysisContext &) = delete;

    /**
     * Export the model as a DOT CFG graph to path.
     */
    void exportDot(const Core::CodeModel &model, const std::string &path,
                   Exporters::DotVerbosity verbosity = Exporters::DotVerbosity::CLEAN);

    /**
     * Export the model as a pretty-printed 3-address-code listing to path.
     */
    void exportPP(const Core::CodeModel &model, const std::string &path);

    /**
     * Serialize the model to the internal JSON format at path.
     */
    void exportJson(const Core::CodeModel &model, const std::string &path);
};

} // namespace CodeListener

#endif /* __cplusplus */
#endif /* ANALYSIS_CONTEXT_HPP */
