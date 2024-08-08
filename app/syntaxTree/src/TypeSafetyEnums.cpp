#ifndef TYPE_SAFETY_ENUMS_CPP
#define TYPE_SAFETY_ENUMS_CPP

#include "TypeSafetyEnums.hpp"

#define clefDefineEnumOp(T, R, op) R operator op(const T a, const T b) { return static_cast<R>((+a) op (+b)); }
#define clefTypeSafetyEnum(T, B)                            \
   uint operator+(const T a) { return static_cast<B>(a); }  \
   clefDefineEnumOp(T,T,+)                                  \
   clefDefineEnumOp(T,T,-)                                  \
   clefDefineEnumOp(T,T,*)                                  \
   clefDefineEnumOp(T,T,%)                                  \
   clefDefineEnumOp(T,T,&)                                  \
   clefDefineEnumOp(T,T,|)                                  \
   clefDefineEnumOp(T,T,^)                                  \
   clefDefineEnumOp(T,bool,==)                              \
   clefDefineEnumOp(T,bool,!=)                              \
   clefDefineEnumOp(T,bool,<)                               \
   clefDefineEnumOp(T,bool,<=)                              \
   clefDefineEnumOp(T,bool,>)                               \
   clefDefineEnumOp(T,bool,>=)                              \
   bool operator!(const T a) { return !+a; }

namespace clef {
   clefTypeSafetyEnum(NodeID_t,uint)
   clefTypeSafetyEnum(TokenID_t,uint)
}

#undef clefDefineEnumOp
#undef clefTypeSafetyEnum

#endif //TYPE_SAFETY_ENUMS_CPP