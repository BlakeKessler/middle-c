#ifndef EXPRESSION_CPP
#define EXPRESSION_CPP

#include "ast-nodes/Expression.hpp"
#include "astNode.hpp"

clef::Expression::Expression(OpID op, index<Type> type, index<ArgList> ctorArgs):
   _op{op},
   _lhsType{NodeType::TYPE},
   _rhsType{NodeType::ARG_LIST},
   _extraType{NodeType::NONE},
   _lhs{type},_rhs{ctorArgs},_extra{} {
      debug_assert(op == OpID::CALL_INVOKE || op == OpID::LIST_INVOKE);
}

#endif //EXPRESSION_CPP