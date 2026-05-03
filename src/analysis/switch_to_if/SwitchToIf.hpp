/**
 * @file SwitchToIf.hpp
 * @author Lukáš Pšeja <xpsejal00@vutbr.cz>
 * @brief Declares the switch-to-if annotation.
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

#include <string>
#include <unordered_map>
#include <vector>

#include "AnnotationBase.hpp"
#include "CodeModel.hpp"
#include "InstructionId.hpp"

namespace CodeListener::AnnotationServices
{

/**
 * Pre-computes the synthetic variable names and block-label allocations needed
 * when unfolding switch/cond instructions into the PP text format.
 */
struct SwitchToIf : public AnnotationBase<SwitchToIf>
{
  private:
    /** One equality-check branch produced when a switch is unfolded into an if-else chain. */
    struct FlatCheck
    {
        /**
         * Case constant rendered in the pretty-printer syntax.
         * e.g. "42", "42U", "(18446744073709551614)"
         */
        std::string formatted_case_constant;

        /** Name of the target block reached when the equality check matches. */
        std::string target_block_name;
    };

    /** Cached textual lowering for one unfolded `SwitchInstruction`. */
    struct UnfoldedSwitch
    {
        /**
         * Synthetic temporary storing the compared switch operand.
         * e.g. "%rGsw0"
         */
        std::string synth_comp_temp;

        /** Name of the default target block reached when no case matches. */
        std::string default_target_block_name;

        /** Flat sequence of equality checks emitted before the default jump. */
        std::vector<FlatCheck> checks;

        /** First synthetic label index reserved for the unfolded branch chain. */
        int synth_label_base;
    };

    /** Cached textual lowering for one unfolded `CondInstruction`. */
    struct UnfoldedCond
    {
        /**
         * Synthetic temporary storing the normalized comparison result.
         * e.g. "%rGcond3"
         */
        std::string synth_comp_temp;
    };

    /**
     * Expand one switch instruction into a flat list of equality checks.
     *
     * @param sw Source switch instruction.
     * @param model Model used to resolve target block names and type information.
     * @param out_default_target Receives the resolved default target block name.
     *
     * @return Flattened list of one-value comparisons emitted by the pretty-printer.
     */
    static std::vector<SwitchToIf::FlatCheck> flattenSwitch(const Core::SwitchInstruction &sw,
                                                            const Core::CodeModel &model,
                                                            std::string &out_default_target);

  public:
    /** Lowered switch metadata keyed by the original switch instruction id. */
    std::unordered_map<Core::InstructionId, UnfoldedSwitch> switches;

    /** Lowered conditional metadata keyed by the original conditional instruction id. */
    std::unordered_map<Core::InstructionId, UnfoldedCond> conds;

    /**
     * Build switch/conditional lowering metadata for the whole model.
     *
     * @param model Model whose control-flow instructions should be analyzed.
     *
     * @return Fully populated switch-to-if annotation.
     */
    static SwitchToIf build(const Core::CodeModel &model);
};

} // namespace CodeListener::AnnotationServices
