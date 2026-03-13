#pragma once

#include <iostream>
#include <string>

#include "Exporter.hpp"

namespace CodeListener::Exporters
{

class DOTExporter : public Exporter
{
  public:
    DOTExporter(std::ostream &os);
    void exportModel(const Core::CodeModel &model) override;

  private:
    std::ostream &os;

    void exportFunction(const Core::CodeModel &model, const Core::Function &func);
    void exportBlock(const Core::CodeModel &model, const Core::Block &block);
    std::string exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr);

    std::string formatOperand(const Core::CodeModel &model, const Core::Operand &op);
    std::string formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc, const std::string &base);
    std::string formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr);

    std::string escape(const std::string &str);

    std::string opCodeToString(Core::OpCode opcode);
    std::string getOperandTypeString(const Core::CodeModel &model, const Core::Operand &op);
};

} // namespace CodeListener::Exporters
