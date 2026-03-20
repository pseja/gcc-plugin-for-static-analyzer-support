#pragma once

namespace CodeListener::Core
{

struct AsmInstruction
{
    // TODO: GCCAdapter doesn't parse operands for ASM yet (old cl doesn't do this at all), but there'll likely be:
    // std::string assembly_string;
    // std::vector<Operand> inputs;
    // std::vector<Operand> outputs;
    // std::vector<Operand> clobbers;
    // std::vector<Operand> labels;
    // this is for now, just a placeholder for the instruction to not get shown in the code like an empty instruction
};

} // namespace CodeListener::Core
