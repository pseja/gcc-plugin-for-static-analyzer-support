// there is an enum conversion warning in a GCC header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"

#include <gcc-plugin.h>
#include <tree.h>            // DECL_NAME
#include <function.h>        // function
#include <gimple.h>          // gimple_location
#include <gimple-iterator.h> // gimple_stmt_iterator, gsi_start_bb, gsi_end_p, gsi_next, gsi_stmt

#pragma GCC diagnostic pop

#include "GCCAdapter.hpp"
#include "NodeId.hpp"
#include "PluginContext.hpp"
#include "SourceLocation.hpp"
#include "Type.hpp"
#include "Operand.hpp"

namespace CodeListener
{

namespace Core
{

GCCAdapter::GCCAdapter(CodeModel &model) : model(model)
{
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, std::string("Initialized GCCAdapter"));
}

TypeKind GCCAdapter::mapTypeTreeToTypeKind(tree &type_tree)
{
    // FIXME: TREE_TYPE?
    switch (TREE_CODE(type_tree))
    {
    case VOID_TYPE:
        return TypeKind::VOID;
    case POINTER_TYPE:
        return TypeKind::POINTER;
    case RECORD_TYPE:
        return TypeKind::STRUCT;
    case UNION_TYPE:
        return TypeKind::UNION;
    case ARRAY_TYPE:
        return TypeKind::ARRAY;
    case FUNCTION_TYPE:
        return TypeKind::FUNCTION;
    case INTEGER_TYPE:
        return TypeKind::INTEGER;
    case BOOLEAN_TYPE:
        return TypeKind::BOOL;
    case ENUMERAL_TYPE:
        return TypeKind::ENUM;
    case REAL_TYPE:
        return TypeKind::REAL;
    case COMPLEX_TYPE:
        return TypeKind::COMPLEX;
    default:
        return TypeKind::UNKNOWN;
    }
}

SourceLocation GCCAdapter::getSourceLocation(location_t location)
{
    if (location == UNKNOWN_LOCATION || location <= BUILTINS_LOCATION)
    {
        // FIXME: before return statement, there is a label to jump to unknown location for some reason
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered an unknown source location");

        return SourceLocation();
    }

    expanded_location eloc = expand_location(location);
    if (eloc.file == nullptr)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered an unknown source location");

        return SourceLocation();
    }

    return SourceLocation(eloc.file, current_function_name, eloc.line, eloc.column);
}

NodeId GCCAdapter::getOrCreateType(tree type_tree)
{
    if (!type_tree)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered a type without a tree representation");

        return NodeId::INVALID;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Maybe processing type tree...");

    if (type_cache.count(type_tree))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Type already processed, reusing existing node");

        return type_cache[type_tree];
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Actually processing type tree...");

    Type type_node;
    type_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(type_tree));

    // pre-insert into cache to handle recursive types
    type_cache[type_tree] = type_node.id;

    if (TYPE_NAME(type_tree) && TREE_CODE(TYPE_NAME(type_tree)) == TYPE_DECL)
    {
        if (DECL_NAME(TYPE_NAME(type_tree)))
        {
            type_node.name = IDENTIFIER_POINTER(DECL_NAME(TYPE_NAME(type_tree)));
        }
    }

    type_node.kind = mapTypeTreeToTypeKind(type_tree);

    type_node.is_const = TYPE_READONLY(type_tree);
    type_node.is_volatile = TYPE_VOLATILE(type_tree);
    type_node.is_atomic = TYPE_ATOMIC(type_tree);
    type_node.is_restrict = TYPE_RESTRICT(type_tree);

    if (TYPE_SIZE(type_tree) && tree_fits_uhwi_p(TYPE_SIZE(type_tree)))
    {
        // FIXME: converting unsigned long to int
        type_node.size_bits = tree_to_uhwi(TYPE_SIZE(type_tree));
        type_node.size_bytes = type_node.size_bits / 8;
    }
    else
    {
        // unknown size
        type_node.size_bits = 0;
        type_node.size_bytes = 0;
    }
    type_node.alignment = TYPE_ALIGN_UNIT(type_tree);

    switch (TREE_CODE(type_tree))
    {
    case INTEGER_TYPE:
        type_node.is_unsigned = TYPE_UNSIGNED(type_tree);
        break;

    case POINTER_TYPE:
    case REFERENCE_TYPE: {
        NodeId inner = getOrCreateType(TREE_TYPE(type_tree));
        type_node.nested_type_ids.push_back(inner);
    }
    break;

    case ARRAY_TYPE: {
        NodeId element_type = getOrCreateType(TREE_TYPE(type_tree));
        type_node.nested_type_ids.push_back(element_type);

        if (TYPE_DOMAIN(type_tree))
        {
            tree max = TYPE_MAX_VALUE(TYPE_DOMAIN(type_tree));
            if (max && tree_fits_uhwi_p(max))
            {
                type_node.array_element_count = tree_to_uhwi(max) + 1;
            }
        }
    }
    break;

    case RECORD_TYPE:
    case UNION_TYPE: {
        type_node.is_struct = (TREE_CODE(type_tree) == RECORD_TYPE);
        type_node.is_union = (TREE_CODE(type_tree) == UNION_TYPE);

        for (tree field = TYPE_FIELDS(type_tree); field; field = DECL_CHAIN(field))
        {
            if (TREE_CODE(field) == FIELD_DECL)
            {
                NodeId field_id = getOrCreateVariable(field);
                type_node.nested_type_ids.push_back(field_id);
            }
        }
    }
    break;

    case FUNCTION_TYPE:
    case METHOD_TYPE: {
        type_node.nested_type_ids.push_back(getOrCreateType(TREE_TYPE(type_tree)));

        for (tree arg = TYPE_ARG_TYPES(type_tree); arg; arg = TREE_CHAIN(arg))
        {
            if (TREE_VALUE(arg) == void_type_node)
                break;
            type_node.nested_type_ids.push_back(getOrCreateType(TREE_VALUE(arg)));
        }
    }
    break;

    case VOID_TYPE:
        // FIXME: does this need to be handled specially?
        break;

    default:
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Unhandled type tree code: " + std::to_string(TREE_CODE(type_tree)));

        break;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug,
        "Processed type node with id: " + toString(type_node.id) + ", name: " + type_node.name +
            ", kind: " + toString(type_node.kind) + ", size: " + std::to_string(type_node.size_bytes) +
            "B, alignment: " + std::to_string(type_node.alignment) +
            "B, qualifiers: " + (type_node.is_const ? "const " : "") + (type_node.is_volatile ? "volatile " : "") +
            (type_node.is_atomic ? "atomic " : "") + (type_node.is_restrict ? "restrict " : "") +
            (type_node.is_unsigned ? "unsigned " : "") + (type_node.is_struct ? "struct " : "") +
            (type_node.is_union ? "union " : "") + ", and " + std::to_string(type_node.nested_type_ids.size()) +
            " nested types with constant array element count of " + std::to_string(type_node.array_element_count));

    model.addType(type_node);

    return type_node.id;
}

NodeId GCCAdapter::getOrCreateVariable(tree variable_tree)
{
    if (!variable_tree)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered a variable without a tree representation");
        return NodeId::INVALID;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Maybe processing variable tree...");

    if (variable_cache.count(variable_tree))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Variable already processed, reusing existing node");

        return variable_cache[variable_tree];
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Actually processing variable tree...");

    Variable variable;
    // FIXME: replace with proper ID generation
    variable.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(variable_tree));

    if (DECL_NAME(variable_tree))
    {
        variable.name = IDENTIFIER_POINTER(DECL_NAME(variable_tree));
    }
    else
    {
        variable.name = "<anonymous>";
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Determined variable name: " + variable.name);

    variable.type_id = getOrCreateType(TREE_TYPE(variable_tree));
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Determined variable type ID: " + toString(variable.type_id));

    variable.source_location = getSourceLocation(DECL_SOURCE_LOCATION(variable_tree));
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Determined variable source location: " + toString(variable.source_location));

    // determine scope
    if (is_global_var(variable_tree))
    {
        variable.scope = Scope::GLOBAL;
    }
    else
    {
        variable.scope = Scope::FUNCTION;
    };
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Determined variable scope: " + toString(variable.scope));

    // FIXME: uncomment after implementing the method
    // model.addVariable(variable);
    variable_cache[variable_tree] = variable.id;

    return variable.id;
}

Operand GCCAdapter::parseOperand(tree operand_tree)
{
    if (!operand_tree)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered an operand without a tree representation");
        return ConstantOperand{NodeId::INVALID, "<invalid>"};
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(DiagnosticLevel::Debug,
                                                                                          "Processing operand tree...");

    if (DECL_P(operand_tree) || (TREE_CODE(operand_tree) == SSA_NAME))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Operand is a variable, processing as variable operand...");

        // TODO: handle SSA_NAMEs properly, they can represent both variables and temporary values
        VariableOperand variable_operand;
        variable_operand.variable_id = getOrCreateVariable(operand_tree);
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Parsed variable operand with ID: " + toString(variable_operand.variable_id));

        return variable_operand;
    }
    else if (CONSTANT_CLASS_P(operand_tree))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Operand is a constant, processing as constant operand...");

        // FIXME: very rough way to represent constants, do proper handling for different constant types
        ConstantOperand constant_operand;
        // FIXME: replace with proper ID generation
        constant_operand.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(operand_tree));
        // TODO: extract actual constant value
        constant_operand.value = "<constant>";

        return constant_operand;
    }
    else
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered an unhandled operand type in gimple statement");
    }

    return ConstantOperand(NodeId::INVALID, "<unhandled>");
}

void GCCAdapter::processFunction(function *fun)
{
    if (!fun || DECL_NAME(fun->decl) == nullptr)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Adapter encountered a function without a name");
        return;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, std::string("Adapter processing function: ") + function_name(fun));

    // save the current function name for source location tracking
    // TODO: maybe bring the function name through function arguments instead of Adapter attribute
    current_function_name = function_name(fun);

    // process function declaration
    tree function_declaration = fun->decl;
    Function function_node;
    // FIXME: replace with proper ID generation
    function_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(function_declaration));
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Function node ID: " + toString(function_node.id));
    function_node.name = function_name(fun);

    tree_node *return_type = TREE_TYPE(function_declaration);
    function_node.return_type_id = getOrCreateType(return_type);
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Function return type ID: " + toString(function_node.return_type_id));

    // process parameters
    for (tree arg = DECL_ARGUMENTS(function_declaration); arg; arg = DECL_CHAIN(arg))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Processing function parameters...");

        function_node.parameter_ids.push_back(getOrCreateVariable(arg));

        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Processed parameter with ID: " + toString(function_node.parameter_ids.back()));
    }

    // process local variables
    basic_block bb;
    FOR_EACH_BB_FN(bb, fun)
    {
        Block block_node;
        // FIXME: replace with proper ID generation
        block_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(bb));
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Block node ID: " + toString(block_node.id));
        block_node.parent_function_id = function_node.id;
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Block node parent function ID: " + toString(block_node.parent_function_id));
        block_node.name = "bb_" + std::to_string(bb->index);
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Block node name: " + block_node.name);

        // process instructions in the block
        for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
        {
            gimple *stmt = gsi_stmt(gsi);

            Instruction instruction_node;
            instruction_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(stmt));
            CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
                DiagnosticLevel::Debug, "Instruction node ID: " + toString(instruction_node.id));
            instruction_node.parent_block_id = block_node.id;
            CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
                DiagnosticLevel::Debug,
                "Instruction node parent block ID: " + toString(instruction_node.parent_block_id));
            instruction_node.source_location = getSourceLocation(gimple_location(stmt));
            CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
                DiagnosticLevel::Debug,
                "Instruction node source location: " + toString(instruction_node.source_location));
            // determine opcode
            enum gimple_code gcode = gimple_code(stmt);
            instruction_node.opcode = gimple_code_name[gcode];
            CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
                DiagnosticLevel::Debug, "Processed instruction with opcode: " + instruction_node.opcode);

            // extract operands based on statement type
            if (is_gimple_assign(stmt))
            {
                tree lhs = gimple_assign_lhs(stmt);
                tree rhs1 = gimple_assign_rhs1(stmt);
                instruction_node.operands.push_back(parseOperand(lhs));
                instruction_node.operands.push_back(parseOperand(rhs1));

                if (gimple_num_ops(stmt) > 2)
                {
                    tree rhs2 = gimple_assign_rhs2(stmt);
                    if (rhs2)
                    {
                        instruction_node.operands.push_back(parseOperand(rhs2));
                    }
                }
            }
            // TODO: handle more statement types
            else
            {
                CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
                    DiagnosticLevel::Warning,
                    "Encountered an unhandled gimple statement type: " + instruction_node.opcode);
            }

            // FIXME: uncomment after implementing the method
            // model.addInstruction(instruction_node);
            block_node.instruction_ids.push_back(instruction_node.id);
        }

        // FIXME: uncomment after implementing the method
        // model.addBlock(block_node);
        function_node.block_ids.push_back(block_node.id);
    }

    // TODO: process CFG edges between blocks

    // FIXME: uncomment after implementing the method
    // model.addFunction(function_node);
}

} // namespace Core

} // namespace CodeListener
