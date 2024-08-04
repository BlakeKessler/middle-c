#pragma once
#ifndef TYPE_SAFETY_ENUMS_HPP
#define TYPE_SAFETY_ENUMS_HPP

#include "CLEF_DEFINES.hpp"

#define clefDefineEnumOp(T, op) T operator op(const T a, const T b);
#define clefTypeSafetyEnum(T, B) \
   enum class T : B {};          \
   uint operator+(const T a);    \
   clefDefineEnumOp(T,+)         \
   clefDefineEnumOp(T,-)         \
   clefDefineEnumOp(T,*)         \
   clefDefineEnumOp(T,%)         \
   clefDefineEnumOp(T,&)         \
   clefDefineEnumOp(T,|)         \
   clefDefineEnumOp(T,^)         \
   clefDefineEnumOp(T,==)        \
   clefDefineEnumOp(T,!=)        \
   clefDefineEnumOp(T,<)         \
   clefDefineEnumOp(T,<=)        \
   clefDefineEnumOp(T,>)         \
   clefDefineEnumOp(T,>=)        \
   bool operator!(const T a);

namespace clef {
   clefTypeSafetyEnum(NodeID_t,uint)
   clefTypeSafetyEnum(TokenID_t,uint)

   constexpr NodeID_t NODE_NIL = static_cast<NodeID_t>(0);
   constexpr TokenID_t TOK_NIL = static_cast<TokenID_t>(0);
}

#undef clefDefineEnumOp
#undef clefTypeSafetyEnum

#endif //TYPE_SAFETY_ENUMS_HPP