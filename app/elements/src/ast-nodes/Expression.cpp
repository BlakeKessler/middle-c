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

mcsl::File& mcsl::write(File& file, const clef::Expr& expr) {
   using namespace clef;
   file.printf(mcsl::FMT("expression: op=%s("), toString(expr.opID()));
   if (+expr.lhsType()) {
      file.printf(mcsl::FMT("lhs: %s{id=%u}"), toString(expr.lhsType()), +expr.lhs());

      if (+expr.rhsType() || +expr.extraType()) { file.printf(mcsl::FMT(", ")); }
   }
   if (+expr.rhsType()) {
      file.printf(mcsl::FMT("rhs: %s{id=%u}"), toString(expr.rhsType()), +expr.rhs());

      if (+expr.extraType()) { file.printf(mcsl::FMT(", ")); }
   }
   if (+expr.extraType()) {
      file.printf(mcsl::FMT("extra: %s{id=%u}"), toString(expr.extraType()), +expr.extra());
   }
   file.printf(mcsl::FMT(")"));
   return file;
}

#endif //EXPRESSION_CPP