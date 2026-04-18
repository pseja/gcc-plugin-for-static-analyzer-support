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
    // One equality-check branch produced when a switch is unfolded into an if-else chain.
    struct FlatCheck
    {
        std::string formatted_case_constant; // e.g. "42", "42U", "(18446744073709551614)"
        std::string target_block_name;
    };

    // Data for one unfolded SwitchInstruction.
    struct UnfoldedSwitch
    {
        std::string synth_comp_temp; // e.g. "%rGsw0"
        std::string default_target_block_name;
        std::vector<FlatCheck> checks;
        int synth_label_base; // first label index: L{base} .. L{base + checks.size() - 1}
    };

    // Data for one unfolded CondInstruction (only when opcode != NONE).
    struct UnfoldedCond
    {
        std::string synth_comp_temp; // e.g. "%rGcond3"
    };

    // Expands one SwitchInstruction's cases into a flat list of single-value equality checks, and resolves the
    // default-case block name.
    static std::vector<SwitchToIf::FlatCheck> flattenSwitch(const Core::SwitchInstruction &sw,
                                                            const Core::CodeModel &model,
                                                            std::string &out_default_target);

  public:
    std::unordered_map<Core::InstructionId, UnfoldedSwitch> switches;
    std::unordered_map<Core::InstructionId, UnfoldedCond> conds;

    static SwitchToIf build(const Core::CodeModel &model);
};

} // namespace CodeListener::AnnotationServices
