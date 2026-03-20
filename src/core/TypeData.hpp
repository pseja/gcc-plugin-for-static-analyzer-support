#pragma once

#include <variant>

#include "UnknownType.hpp"
#include "VoidType.hpp"
#include "EnumType.hpp"
#include "IntegerType.hpp"
#include "FloatType.hpp"
#include "BoolType.hpp"
#include "PointerType.hpp"
#include "ArrayType.hpp"
#include "StructType.hpp"
#include "UnionType.hpp"
#include "FunctionType.hpp"

namespace CodeListener::Core
{

using TypeData = std::variant<UnknownType, VoidType, EnumType, IntegerType, FloatType, BoolType, PointerType, ArrayType,
                              StructType, UnionType, FunctionType>;

} // namespace CodeListener::Core
