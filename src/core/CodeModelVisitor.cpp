#include "CodeModelVisitor.hpp"

#include "Block.hpp"
#include "CodeModel.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "Variable.hpp"

namespace CodeListener::Core
{

void CodeModelVisitor::walk(const CodeModel &model)
{
    onBeginModel(model);

    for (const auto &type : model.getTypes())
    {
        onVisitType(model, type);
    }

    for (const auto &var : model.getVariables())
    {
        onVisitVariable(model, var);
    }

    for (const auto &func : model.getFunctions())
    {
        onBeginFunction(model, func);

        for (const auto block_id : func.block_ids)
        {
            const Block &block = *model.getBlock(block_id);
            onBeginBlock(model, block);

            for (const auto instr_id : block.instruction_ids)
            {
                const Instruction &instr = *model.getInstruction(instr_id);
                onBeforeInstruction(model, instr);
                onVisitInstruction(model, instr);
                onAfterInstruction(model, instr);
            }

            onEndBlock(model, block);
        }

        onEndFunction(model, func);
    }

    onEndModel(model);
}

void CodeModelVisitor::onBeginModel(const CodeModel &)
{
}
void CodeModelVisitor::onEndModel(const CodeModel &)
{
}

void CodeModelVisitor::onVisitType(const CodeModel &, const Type &)
{
}
void CodeModelVisitor::onVisitVariable(const CodeModel &, const Variable &)
{
}

void CodeModelVisitor::onBeginFunction(const CodeModel &, const Function &)
{
}
void CodeModelVisitor::onEndFunction(const CodeModel &, const Function &)
{
}

void CodeModelVisitor::onBeginBlock(const CodeModel &, const Block &)
{
}
void CodeModelVisitor::onEndBlock(const CodeModel &, const Block &)
{
}

void CodeModelVisitor::onBeforeInstruction(const CodeModel &, const Instruction &)
{
}
void CodeModelVisitor::onVisitInstruction(const CodeModel &, const Instruction &)
{
}
void CodeModelVisitor::onAfterInstruction(const CodeModel &, const Instruction &)
{
}

} // namespace CodeListener::Core
