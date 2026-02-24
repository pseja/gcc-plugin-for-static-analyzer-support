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

    if (variable_cache.count(variable_tree))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Variable already processed, reusing existing node");

        return variable_cache[variable_tree];
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Processing a new variable...");

    Variable variable;
    variable.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(variable_tree));

    // pre-insert into cache
    variable_cache[variable_tree] = variable.id;

    // resolve name
    if (DECL_NAME(variable_tree))
    {
        const char *name = IDENTIFIER_POINTER(DECL_NAME(variable_tree));
        variable.name = name ? name : "<anonymous>";
    }
    else if (TREE_CODE(variable_tree) == SSA_NAME)
    {
        // for SSA names, try to get the underlying var name
        tree var = SSA_NAME_VAR(variable_tree);
        if (var && DECL_NAME(var))
        {
            const char *name = IDENTIFIER_POINTER(DECL_NAME(var));
            variable.name =
                std::string(name ? name : "<anonymous>") + "_" + std::to_string(SSA_NAME_VERSION(variable_tree));
        }
        else
        {
            variable.name = "ssa_" + std::to_string(SSA_NAME_VERSION(variable_tree));
        }
    }
    else
    {
        variable.name = "<anonymous>";
    }

    variable.type_id = getOrCreateType(TREE_TYPE(variable_tree));

    if (DECL_P(variable_tree))
    {
    variable.source_location = getSourceLocation(DECL_SOURCE_LOCATION(variable_tree));

    // determine scope
    if (is_global_var(variable_tree))
    {
        variable.scope = Scope::GLOBAL;
        }
        else if (TREE_CODE(variable_tree) == PARM_DECL)
        {
            variable.scope = Scope::FUNCTION;
        }
        else
        {
            variable.scope = Scope::FUNCTION;
        }

        // determine storage duration and linkage
        if (TREE_CODE(variable_tree) == PARM_DECL)
        {
            variable.storage_duration = StorageDuration::AUTO;
            variable.linkage = Linkage::NONE;
        }
        else if (TREE_STATIC(variable_tree))
        {
            variable.storage_duration = StorageDuration::STATIC;
            variable.linkage = TREE_PUBLIC(variable_tree) ? Linkage::EXTERNAL : Linkage::INTERNAL;
        }
        else if (DECL_EXTERNAL(variable_tree))
        {
            variable.storage_duration = StorageDuration::EXTERN;
            variable.linkage = Linkage::EXTERNAL;
        }
        else if (DECL_REGISTER(variable_tree))
        {
            variable.storage_duration = StorageDuration::REGISTER;
            variable.linkage = Linkage::NONE;
        }
        else
        {
            variable.storage_duration = StorageDuration::AUTO;
            variable.linkage = Linkage::NONE;
        }

        // thread-local storage
        if (DECL_THREAD_LOCAL_P(variable_tree))
        {
            variable.storage_duration = StorageDuration::THREAD_LOCAL;
        }

        // bitfield information for struct fields
        if (TREE_CODE(variable_tree) == FIELD_DECL && DECL_BIT_FIELD(variable_tree))
        {
            variable.is_bitfield = true;
            if (DECL_SIZE(variable_tree) && tree_fits_uhwi_p(DECL_SIZE(variable_tree)))
            {
                variable.bitfield_size = tree_to_uhwi(DECL_SIZE(variable_tree));
            }
            if (DECL_FIELD_BIT_OFFSET(variable_tree) && tree_fits_uhwi_p(DECL_FIELD_BIT_OFFSET(variable_tree)))
            {
                variable.bitfield_offset = tree_to_uhwi(DECL_FIELD_BIT_OFFSET(variable_tree));
            }
        }
    }
    else
    {
        // for SSA names and other non-declaration variables, we may not have good source location or scope information
        variable.source_location = SourceLocation();
        variable.scope = Scope::FUNCTION;
        variable.storage_duration = StorageDuration::AUTO;
        variable.linkage = Linkage::NONE;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug,
        "Processed variable node with id: " + toString(variable.id) + ", name: " + variable.name +
            ", type id: " + toString(variable.type_id) + ", source location: " + toString(variable.source_location) +
            ", scope: " + toString(variable.scope) + ", storage duration: " + toString(variable.storage_duration) +
            ", linkage: " + toString(variable.linkage) + ", qualifiers: " +
            (variable.is_bitfield ? "bitfield (" + std::to_string(variable.bitfield_size) +
                                        " bits, offset: " + std::to_string(variable.bitfield_offset) + " bits)"
                                  : ""));

    model.addVariable(variable);

    return variable.id;
}

Operand GCCAdapter::parseOperand(tree operand_tree)
{
    if (!operand_tree)
    {
        return ConstantOperand{NodeId::INVALID, "<null>"};
    }

    // handle constants
    if (CONSTANT_CLASS_P(operand_tree))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Processing operand as constant operand...");

        ConstantOperand constant_operand;
        constant_operand.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(operand_tree));

        if (TREE_CODE(operand_tree) == INTEGER_CST)
        {
            if (TYPE_UNSIGNED(TREE_TYPE(operand_tree)))
            {
                constant_operand.value = std::to_string(tree_to_uhwi(operand_tree));
            }
            else
            {
                constant_operand.value = std::to_string(tree_to_shwi(operand_tree));
            }
        }
        else if (TREE_CODE(operand_tree) == STRING_CST)
        {
            constant_operand.value = std::string("\"") + TREE_STRING_POINTER(operand_tree) + "\"";
        }
        else if (TREE_CODE(operand_tree) == REAL_CST)
        {
            char buf[64];
            real_to_decimal(buf, &TREE_REAL_CST(operand_tree), sizeof(buf), 0, 1);
            constant_operand.value = buf;
        }
        else
        {
            constant_operand.value = "<unknown_constant>";
        }

        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Parsed constant operand with value: " + constant_operand.value);

        return constant_operand;
    }

    // handle base variables
    if (DECL_P(operand_tree) || TREE_CODE(operand_tree) == SSA_NAME)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Processing operand as variable operand...");

        VariableOperand var_op;
        var_op.variable_id = getOrCreateVariable(operand_tree);
        return var_op;
    }

    // handle recursive access paths
    if (TREE_CODE(operand_tree) == COMPONENT_REF)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Recursively parsing component reference operand...");

        Operand base_op = parseOperand(TREE_OPERAND(operand_tree, 0));
        if (std::holds_alternative<VariableOperand>(base_op))
        {
            VariableOperand &var_op = std::get<VariableOperand>(base_op);
            Accessor acc;
            acc.kind = AccessorKind::FIELD;
            acc.target_field_id = getOrCreateVariable(TREE_OPERAND(operand_tree, 1));

            var_op.access_path.push_back(acc);
            return var_op;
        }
        return base_op;
    }

    if (TREE_CODE(operand_tree) == ARRAY_REF)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Recursively parsing array reference operand...");

        Operand base_op = parseOperand(TREE_OPERAND(operand_tree, 0));
        if (std::holds_alternative<VariableOperand>(base_op))
        {
            VariableOperand &var_op = std::get<VariableOperand>(base_op);
            Accessor acc;
            acc.kind = AccessorKind::ARRAY;

            tree index = TREE_OPERAND(operand_tree, 1);
            Operand index_op = parseOperand(index);

            if (std::holds_alternative<ConstantOperand>(index_op))
            {
                acc.index_operand_id = std::get<ConstantOperand>(index_op).id;
    }
    else
            {
                acc.index_operand_id = std::get<VariableOperand>(index_op).variable_id;
            }

            var_op.access_path.push_back(acc);
            return var_op;
        }
        return base_op;
    }

    if (TREE_CODE(operand_tree) == MEM_REF)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Recursively parsing memory reference operand...");

        Operand ptr_op = parseOperand(TREE_OPERAND(operand_tree, 0));
        if (std::holds_alternative<VariableOperand>(ptr_op))
        {
            VariableOperand &var_op = std::get<VariableOperand>(ptr_op);

            tree offset_tree = TREE_OPERAND(operand_tree, 1);
            if (offset_tree && !integer_zerop(offset_tree))
            {
                Accessor off_acc;
                off_acc.kind = AccessorKind::OFFSET;
                Operand off_op = parseOperand(offset_tree);
                if (std::holds_alternative<ConstantOperand>(off_op))
                {
                    off_acc.index_operand_id = std::get<ConstantOperand>(off_op).id;
                }
                var_op.access_path.push_back(off_acc);
            }

            Accessor deref_acc;
            deref_acc.kind = AccessorKind::DEREF;
            var_op.access_path.push_back(deref_acc);

            return var_op;
        }
        return ptr_op;
    }

    if (TREE_CODE(operand_tree) == INDIRECT_REF)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Recursively parsing indirect reference operand...");

        Operand ptr_op = parseOperand(TREE_OPERAND(operand_tree, 0));
        if (std::holds_alternative<VariableOperand>(ptr_op))
        {
            VariableOperand &var_op = std::get<VariableOperand>(ptr_op);
            Accessor acc;
            acc.kind = AccessorKind::DEREF;
            var_op.access_path.push_back(acc);
            return var_op;
        }
        return ptr_op;
    }

    if (TREE_CODE(operand_tree) == ADDR_EXPR)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Recursively parsing address of operand...");

        Operand x_op = parseOperand(TREE_OPERAND(operand_tree, 0));
        if (std::holds_alternative<VariableOperand>(x_op))
        {
            VariableOperand &var_op = std::get<VariableOperand>(x_op);
            Accessor acc;
            acc.kind = AccessorKind::ADDRESS_OF;
            acc.target_field_id = getOrCreateType(TREE_TYPE(operand_tree));
            var_op.access_path.push_back(acc);
            return var_op;
        }
        return x_op;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Operand tree code not handled: " + std::to_string(TREE_CODE(operand_tree)));

    return ConstantOperand{NodeId::INVALID, "<unhandled_operand>"};
}
}

void GCCAdapter::processFunction(function *fun)
{
    if (!fun || !fun->decl || DECL_NAME(fun->decl) == nullptr)
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

    Function function_node;
    function_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(fun->decl));
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Function node ID: " + toString(function_node.id));
    function_node.name = current_function_name;

    if (TREE_TYPE(fun->decl))
    {
        tree_node *return_type = TREE_TYPE(TREE_TYPE(fun->decl));
        if (return_type)
        {
    function_node.return_type_id = getOrCreateType(return_type);
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Function return type ID: " + toString(function_node.return_type_id));
        }
    }

    // process parameters
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Processing function parameters...");
    for (tree arg = DECL_ARGUMENTS(fun->decl); arg; arg = DECL_CHAIN(arg))
    {
        NodeId param_id = getOrCreateVariable(arg);
        function_node.parameter_ids.push_back(param_id);
    }
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Processed function parameters");

    // process local variables
    if (fun->local_decls)
    {
        tree var;
        unsigned i;
        FOR_EACH_VEC_SAFE_ELT(fun->local_decls, i, var)
        {
            if (TREE_CODE(var) != VAR_DECL)
            {
                continue;
            }

            // filter out artificial variables or those not in the source
            if (DECL_ARTIFICIAL(var))
            {
                continue;
            }

            NodeId var_id = getOrCreateVariable(var);
            function_node.local_variable_ids.push_back(var_id);
        }
    }

    // process basic blocks
    basic_block bb;
    FOR_EACH_BB_FN(bb, fun)
    {
        Block block_node;
        block_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(bb));
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Block node ID: " + toString(block_node.id));
        block_node.parent_function_id = function_node.id;
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Block node parent function ID: " + toString(block_node.parent_function_id));
        block_node.name = "bb_" + std::to_string(bb->index);

        // process CFG edges
        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE(e, ei, bb->preds)
        {
            NodeId pred_id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(e->src));
            block_node.predecesor_block_ids.push_back(pred_id);
        }
        FOR_EACH_EDGE(e, ei, bb->succs)
        {
            NodeId succ_id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(e->dest));
            block_node.successor_block_ids.push_back(succ_id);
        }

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

            // determine opcode and kind
            enum gimple_code gcode = gimple_code(stmt);
            instruction_node.opcode_name = gimple_code_name[gcode];
            CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
                DiagnosticLevel::Debug, "Processed instruction with opcode: " + instruction_node.opcode_name);

            // try to enhance location if missing (for labels and returns)
            location_t loc = gimple_location(stmt);
            if (gcode == GIMPLE_LABEL && (loc == UNKNOWN_LOCATION || loc <= BUILTINS_LOCATION))
            {
                tree label = gimple_label_label(as_a<glabel *>(stmt));
                if (label && DECL_SOURCE_LOCATION(label) != UNKNOWN_LOCATION)
                {
                    loc = DECL_SOURCE_LOCATION(label);
                }
            }
            if (gcode == GIMPLE_RETURN && (loc == UNKNOWN_LOCATION || loc <= BUILTINS_LOCATION))
            {
                if (fun && fun->function_end_locus != UNKNOWN_LOCATION)
                {
                    loc = fun->function_end_locus;
                }
            }
            instruction_node.source_location = getSourceLocation(loc);

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
