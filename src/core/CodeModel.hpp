#pragma once

// #include <memory> // std::unique_ptr
// #include <unordered_map> // std::unordered_map
#include <deque> // std::deque

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

    // Type *getType(TypeId id);
    // Variable *getVariable(VariableId id);
    // Function *getFunction(FunctionId id);
    // Block *getBlock(BlockId id);
    // Instruction *getInstruction(InstructionId id);

    const std::deque<Type> &getTypes() const;
    const std::deque<Variable> &getVariables() const;
    const std::deque<Function> &getFunctions() const;
    const std::deque<Block> &getBlocks() const;
    const std::deque<Instruction> &getInstructions() const;

    // // flat range views
    // auto types() const;
    // auto variables() const;
    // auto functions() const;
    // auto blocks() const;
    // auto instructions() const;

    // auto parametersOf(const Function &func) const;
    // auto blocksOf(const Function &func) const;
    // auto instructionsOf(const Block &block) const;
    // auto instructionsOf(const Function &func) const;

    // template <typename T>
    // const T *getAnnotation(NodeId target_id, const std::string &key) const;

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

    // template <typename T>
    // void attachAnnotation(NodeId target_id, const std::string &key, std::unique_ptr<T> data);

  private:
    // flat storage
    std::deque<Type> types_pool;
    std::deque<Variable> variables_pool;
    std::deque<Function> functions_pool;
    std::deque<Block> blocks_pool;
    std::deque<Instruction> instructions_pool;

    // annotations
    // std::unordered_map<NodeId, std::unordered_map<std::string, std::unique_ptr<AnnotationServices::AnnotationBase>>>
    //     annotations;
};

} // namespace CodeListener::Core
