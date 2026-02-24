#pragma once

#include <memory>        // std::unique_ptr
#include <unordered_map> // std::unordered_map

#include "AnnotationBase.hpp"
#include "Block.hpp"
#include "Function.hpp"
#include "Instruction.hpp"
#include "NodeId.hpp"
#include "Type.hpp"
#include "Variable.hpp"

namespace CodeListener
{

namespace Core
{

class CodeModel
{
  public:
    // Query System API
    // TODO: chaining with C++23 ranges
    const Type *getType(NodeId id) const;
    const Variable *getVariable(NodeId id) const;
    const Function *getFunction(NodeId id) const;
    const Block *getBlock(NodeId id) const;
    const Instruction *getInstruction(NodeId id) const;

    const std::unordered_map<NodeId, Type> &getTypes() const;
    const std::unordered_map<NodeId, Variable> &getVariables() const;
    const std::unordered_map<NodeId, Function> &getFunctions() const;
    const std::unordered_map<NodeId, Block> &getBlocks() const;
    const std::unordered_map<NodeId, Instruction> &getInstructions() const;

    // Model Builder API
    void addType(Type type);
    void addVariable(Variable var);
    void addFunction(Function func);
    void addBlock(Block block);
    void addInstruction(Instruction instr);

    template <typename T> void attachAnnotation(NodeId target_id, const std::string &key, std::unique_ptr<T> data);
    template <typename T> const T *getAnnotation(NodeId target_id, const std::string &key) const;

  private:
    // flat storage
    std::unordered_map<NodeId, Type> types;
    std::unordered_map<NodeId, Variable> variables;
    std::unordered_map<NodeId, Function> functions;
    std::unordered_map<NodeId, Block> blocks;
    std::unordered_map<NodeId, Instruction> instructions;

    // annotations
    std::unordered_map<NodeId, std::unordered_map<std::string, std::unique_ptr<AnnotationServices::AnnotationBase>>>
        annotations;
};

} // namespace Core

} // namespace CodeListener
