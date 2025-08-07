#ifndef EXPRESSION_CPP
#define EXPRESSION_CPP

#include "ast-nodes/Expr.hpp"
#include "astNode.hpp"
#include "SyntaxTree.hpp"
#include "pretty-print.hpp"
#include "io.hpp"

clef::Expr clef::Expr::makeTernary(SyntaxTree& tree, index<astNode> cond, index<astNode> ifExpr, index<astNode> elseExpr) {
   return Expr{
      OpID::TERNARY_INVOKE,
      tree[cond].nodeType(), tree[ifExpr].nodeType(), tree[elseExpr].nodeType(),
      +cond, +ifExpr, +elseExpr
   };
}


mcsl::File& mcsl::write(File& file, const clef::Expr& expr) {
   using namespace clef;
   file.printf(mcsl::FMT("expression: op=%s("), toString(expr.opID()));
   if (+expr.lhsType() || expr.lhs()) {
      file.printf(mcsl::FMT("lhs: %s{id=%u}"), toString(expr.lhsType()), +expr.lhs());

      if (+expr.rhsType() || +expr.extraType()) { file.write(mcsl::FMT(", ")); }
   }
   if (+expr.rhsType() || expr.rhs()) {
      file.printf(mcsl::FMT("rhs: %s{id=%u}"), toString(expr.rhsType()), +expr.rhs());

      if (+expr.extraType()) { file.write(mcsl::FMT(", ")); }
   }
   if (+expr.extraType() || expr.extra()) {
      file.printf(mcsl::FMT("extra: %s{id=%u}"), toString(expr.extraType()), +expr.extra());
   }
   if (+expr.extraType2() || expr.extra2()) {
      file.printf(mcsl::FMT("extra: %s{id=%u}"), toString(expr.extraType2()), +expr.extra2());
   }
   file.write(')');
   return file;
}

#endif //EXPRESSION_CPP