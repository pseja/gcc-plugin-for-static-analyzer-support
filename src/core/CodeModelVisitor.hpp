#pragma once

#include "Block.hpp"
#include "CodeModel.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "Variable.hpp"

namespace CodeListener::Core
{

class CodeModelVisitor
{
  public:
    virtual ~CodeModelVisitor() = default;

    void walk(const CodeModel &model);

  protected:
    virtual void onBeginModel(const CodeModel &);
    virtual void onEndModel(const CodeModel &);

    virtual void onVisitType(const CodeModel &, const Type &);
    virtual void onVisitVariable(const CodeModel &, const Variable &);

    virtual void onBeginFunction(const CodeModel &, const Function &);
    virtual void onEndFunction(const CodeModel &, const Function &);

    virtual void onBeginBlock(const CodeModel &, const Block &);
    virtual void onEndBlock(const CodeModel &, const Block &);

    virtual void onBeforeInstruction(const CodeModel &, const Instruction &);
    virtual void onVisitInstruction(const CodeModel &, const Instruction &);
    virtual void onAfterInstruction(const CodeModel &, const Instruction &);
};

} // namespace CodeListener::Core
