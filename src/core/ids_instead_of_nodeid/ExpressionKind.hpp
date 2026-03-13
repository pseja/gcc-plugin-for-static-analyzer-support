#pragma once

namespace CodeListener::Core
{

enum class ExpressionKind
{
    CONSTANT,
    VARIABLE,
    ARRAY_ACCESS,
    FIELD_ACCESS,
    DEREFERENCE,
    // ADDRESS_OF,
    // FUNCTION_CALL,
    // TYPE_CAST,
    // BINARY_OPERATION,
    // UNARY_OPERATION,
    // CONDITIONAL,
};

} // namespace CodeListener::Core
