#ifndef EXPRESSION_CPP
#define EXPRESSION_CPP

#include "ast-nodes/Expression.hpp"
#include "astNode.hpp"
#include "pretty-print.hpp"
#include "io.hpp"

clef::Expression::Expression(OpID op, index<Type> type, index<ArgList> ctorArgs):
   _op{op},
   _lhsType{NodeType::TYPE},
   _rhsType{NodeType::ARG_LIST},
   _extraType{NodeType::NONE},
   _lhs{type},_rhs{ctorArgs},_extra{} {
      debug_assert(op == OpID::CALL_INVOKE || op == OpID::LIST_INVOKE);
}

void clef::Expression::printf() const {
   mcsl::printf(mcsl::FMT("expression: op=%s("), toString(opID()));
   if (+lhsType()) {
      mcsl::printf(mcsl::FMT("lhs: %s{id=%u}"), toString(lhsType()), +lhs());

      if (+rhsType() || +extraType()) { mcsl::printf(mcsl::FMT(", ")); }
   }
   if (+rhsType()) {
      mcsl::printf(mcsl::FMT("rhs: %s{id=%u}"), toString(rhsType()), +rhs());

      if (+extraType()) { mcsl::printf(mcsl::FMT(", ")); }
   }
   if (+extraType()) {
      mcsl::printf(mcsl::FMT("extra: %s{id=%u}"), toString(extraType()), +extra());
   }
   mcsl::printf(mcsl::FMT(")"));
}

#endif //EXPRESSION_CPP