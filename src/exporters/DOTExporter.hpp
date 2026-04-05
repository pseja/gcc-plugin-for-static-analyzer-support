#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "Exporter.hpp"

namespace CodeListener::Exporters
{

class DOTExporter : public Exporter
{
  public:
    explicit DOTExporter(std::ostream &os);
    explicit DOTExporter(const std::string &filepath);

  protected:
    void onBeginModel(const Core::CodeModel &model) override;
    void onEndModel(const Core::CodeModel &model) override;

    void onBeginFunction(const Core::CodeModel &model, const Core::Function &func) override;
    void onEndFunction(const Core::CodeModel &model, const Core::Function &func) override;

    void onBeginBlock(const Core::CodeModel &model, const Core::Block &block) override;
    void onEndBlock(const Core::CodeModel &model, const Core::Block &block) override;

    void onVisitInstruction(const Core::CodeModel &model, const Core::Instruction &instr) override;

  private:
    std::ofstream file_os;
    std::ostream &os;

    void emitBlockEdges(const Core::CodeModel &model, const Core::Block &block);

    std::string exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr);
    std::string formatOperand(const Core::CodeModel &model, const Core::Operand &op);
    std::string formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc, const std::string &base);
    std::string formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr);

    std::string escape(const std::string &str);
    std::string opCodeToString(Core::OpCode opcode);
};

} // namespace CodeListener::Exporters
