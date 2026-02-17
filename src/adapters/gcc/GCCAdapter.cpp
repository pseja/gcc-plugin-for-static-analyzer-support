#include <gcc-plugin.h>
#include <function.h> // function
#include <tree.h> // DECL_NAME

#include "GCCAdapter.hpp"
#include "NodeId.hpp"
#include "PluginContext.hpp"
#include "SourceLocation.hpp"
#include "Type.hpp"

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
    if (location == UNKNOWN_LOCATION)
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Warning, "Encountered an unknown source location");

        return SourceLocation();
    }

    expanded_location eloc = expand_location(location);

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
    // FIXME: replace with proper ID generation
    type_node.id = static_cast<NodeId>(reinterpret_cast<uintptr_t>(type_tree));
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Type node ID: " + toString(type_node.id));

    type_node.kind = mapTypeTreeToTypeKind(type_tree);
    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Mapped type tree to TypeKind: " + toString(type_node.kind));

    if (TYPE_SIZE_UNIT(type_tree) && tree_fits_uhwi_p(TYPE_SIZE_UNIT(type_tree)))
    {
        // FIXME: converting unsigned long to int
        type_node.size = tree_to_uhwi(TYPE_SIZE_UNIT(type_tree));
    }
    else
    {
        // unknown size
        type_node.size = 0;
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Determined type size: " + std::to_string(type_node.size) + "B");

    // recursively process nested types
    if (POINTER_TYPE_P(type_tree))
    {
        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Processing pointer type, getting pointee type...");

        NodeId inner = getOrCreateType(TREE_TYPE(type_tree));
        type_node.nested_type_ids.push_back(inner);

        CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
            DiagnosticLevel::Debug, "Pointer type has pointee type with ID: " + toString(inner));
    }

    CompilerAbstractionLayer::PluginContext::getInstance().getDiagnosticReporter().report(
        DiagnosticLevel::Debug, "Final type node ID: " + toString(type_node.id));

    // FIXME: uncomment after implementing the method
    // model.addType(type_node);
    type_cache[type_tree] = type_node.id;

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

    // TODO: process local variables
}

} // namespace Core

} // namespace CodeListener
