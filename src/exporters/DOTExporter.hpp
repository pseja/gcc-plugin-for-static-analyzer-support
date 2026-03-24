#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "AnalysisManager.hpp"
#include "Exporter.hpp"

namespace CodeListener::Exporters
{

class DOTExporter : public Exporter
{
  public:
    explicit DOTExporter(std::ostream &os, AnnotationServices::AnalysisManager *manager = nullptr);
    explicit DOTExporter(const std::string &filepath, AnnotationServices::AnalysisManager *manager = nullptr);

    void exportModel(const Core::CodeModel &model) override;

  private:
    std::ofstream file_os;
    std::ostream &os;
    AnnotationServices::AnalysisManager *analysis_manager;

    void exportFunction(const Core::CodeModel &model, const Core::Function &func);
    void exportBlock(const Core::CodeModel &model, const Core::Block &block);
    std::string exportInstruction(const Core::CodeModel &model, const Core::Instruction &instr);

    std::string formatOperand(const Core::CodeModel &model, const Core::Operand &op);
    std::string formatAccessor(const Core::CodeModel &model, const Core::Accessor &acc, const std::string &base);
    std::string formatInstructionText(const Core::CodeModel &model, const Core::Instruction &instr);

    std::string escape(const std::string &str);

    std::string opCodeToString(Core::OpCode opcode);
};

} // namespace CodeListener::Exporters
