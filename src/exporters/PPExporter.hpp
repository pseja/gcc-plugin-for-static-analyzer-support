#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "Exporter.hpp"
#include "Operand.hpp"

namespace CodeListener::Exporters
{

/**
 * Exports the CodeModel in the 3AC "pp" text format produced by the old cl_pp.cc listener.
 * This is the primary replacement for libcl.a's "pp" listener.
 *
 * Switch instructions are unfolded to if-else chains inline (replicating the
 * unfold_switch filter from the old libcl.a), so that the output matches the
 * format expected by the comparison tests.
 */
class PPExporter : public Exporter
{
  public:
    explicit PPExporter(std::ostream &os);
    explicit PPExporter(const std::string &filepath);

  protected:
    void onBeginFunction(const Core::CodeModel &model, const Core::Function &func) override;
    void onEndFunction(const Core::CodeModel &model, const Core::Function &func) override;

    bool shouldVisitBlock(const Core::CodeModel &model, const Core::Block &block) override;
    void onBeginBlock(const Core::CodeModel &model, const Core::Block &block) override;

    void onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &inst) override;

    void onEndModel(const Core::CodeModel &model) override;

  private:
    std::ofstream file_os;
    std::ostream &os;

    // tracks whether the current block already has an explicit terminator
    bool block_has_terminator = false;

    // counter for synthetic block labels (L1000000, L1000001, ...) in switch unfolding
    int synth_label_counter = 0;

    // counter for synthetic temp-variable names per switch within a function
    int switch_counter = 0;

    // operand formatting
    std::string fmtOperand(const Core::Operand &op, const Core::CodeModel &model) const;
    std::string fmtVar(Core::VariableId id, const std::vector<Core::Accessor> &accessors,
                       const Core::CodeModel &model) const;
    std::string fmtConst(const Core::ConstantOperand &cst, const Core::CodeModel &model) const;

    // instruction helpers
    static std::string binopSym(Core::OpCode op);
    static bool isBinOp(Core::OpCode op);

    // emit switch unfolded as a sequence of equality checks + if-else jumps
    // TODO: figure out, how to do this with the help of the annotations instead
    void emitSwitchUnfolded(const Core::SwitchInstruction &sw, const Core::CodeModel &model);

    std::string nextSynthLabel();

    void onEndBlock(const Core::CodeModel &model, const Core::Block &block) override;
};

} // namespace CodeListener::Exporters
