#ifndef EXPRESSION_CPP
#define EXPRESSION_CPP

#include "ast-nodes/Expression.hpp"
#include "astNode.hpp"
#include "pretty-print.hpp"

#include <cstdio>

clef::Expression::Expression(OpID op, index<Type> type, index<ArgList> ctorArgs):
   _op{op},
   _lhsType{NodeType::TYPE},
   _rhsType{NodeType::ARG_LIST},
   _extraType{NodeType::NONE},
   _lhs{type},_rhs{ctorArgs},_extra{} {
      debug_assert(op == OpID::CALL_INVOKE || op == OpID::LIST_INVOKE);
}

void clef::Expression::printf() const {
   std::printf("expression: op=%s(", toString(opID()));
   if (+lhsType()) {
      std::printf("lhs: %s{id=%u}", toString(lhsType()), +lhs());

      if (+rhsType() || +extraType()) { std::printf(", "); }
   }
   if (+rhsType()) {
      std::printf("rhs: %s{id=%u}", toString(rhsType()), +rhs());

      if (+extraType()) { std::printf(", "); }
   }
   if (+extraType()) {
      std::printf("extra: %s{id=%u}", toString(extraType()), +extra());
   }
   std::printf(")");
}

#endif //EXPRESSION_CPP