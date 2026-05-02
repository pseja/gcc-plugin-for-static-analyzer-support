#pragma once

// #include <memory> // std::unique_ptr
// #include <unordered_map> // std::unordered_map
#include <deque>  // std::deque
#include <ranges> // std::views::transform, std::views::join

// #include "AnnotationBase.hpp"
#include "Block.hpp"
#include "BlockId.hpp"
#include "Function.hpp"
#include "FunctionId.hpp"
#include "Instruction.hpp"
#include "InstructionId.hpp"
#include "Type.hpp"
#include "TypeId.hpp"
#include "Variable.hpp"
#include "VariableId.hpp"

namespace CodeListener::Core
{

class CodeModel
{
  public:
    // --- Query System API ---
    const Type *getType(TypeId id) const;
    const Variable *getVariable(VariableId id) const;
    const Function *getFunction(FunctionId id) const;
    const Block *getBlock(BlockId id) const;
    const Instruction *getInstruction(InstructionId id) const;

    Type *getTypeMutable(TypeId id);
    Variable *getVariableMutable(VariableId id);
    Function *getFunctionMutable(FunctionId id);
    Block *getBlockMutable(BlockId id);
    Instruction *getInstructionMutable(InstructionId id);

    const std::string &getFilename() const;
    void setFilename(const std::string &filename);

    // flat range views
    [[nodiscard]] auto types() const
    {
        return std::views::all(types_pool);
    }
    [[nodiscard]] auto variables() const
    {
        return std::views::all(variables_pool);
    }
    [[nodiscard]] auto functions() const
    {
        return std::views::all(functions_pool);
    }
    [[nodiscard]] auto blocks() const
    {
        return std::views::all(blocks_pool);
    }
    [[nodiscard]] auto instructions() const
    {
        return std::views::all(instructions_pool);
    }
    [[nodiscard]] auto parametersOf(const Function &func) const
    {
        return func.parameter_ids |
               std::views::transform([this](VariableId id) -> const Variable & { return variables_pool[id.index]; });
    }
    [[nodiscard]] auto blocksOf(const Function &func) const
    {
        return func.block_ids |
               std::views::transform([this](BlockId id) -> const Block & { return blocks_pool[id.index]; });
    }
    [[nodiscard]] auto instructionsOf(const Block &block) const
    {
        return block.instruction_ids | std::views::transform([this](InstructionId id) -> const Instruction & {
                   return instructions_pool[id.index];
               });
    }
    [[nodiscard]] auto instructionsOf(const Function &func) const
    {
        return blocksOf(func) | std::views::transform([this](const Block &b) { return instructionsOf(b); }) |
               std::views::join;
    }

    // --- Model Builder API ---
    Type *createType();
    Variable *createVariable();
    Function *createFunction();
    Block *createBlock();
    Instruction *createInstruction();

    void addType(Type *type);
    void addVariable(Variable *variable);
    void addFunction(Function *function);
    void addBlock(Block *block);
    void addInstruction(Instruction *instruction);

  private:
    // flat storage
    std::deque<Type> types_pool;
    std::deque<Variable> variables_pool;
    std::deque<Function> functions_pool;
    std::deque<Block> blocks_pool;
    std::deque<Instruction> instructions_pool;

    std::string filename;
};

} // namespace CodeListener::Core
