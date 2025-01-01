#ifndef EXPRESSION_CPP
#define EXPRESSION_CPP

#include "ast-nodes/Expression.hpp"
#include "astNode.hpp"

void clef::Expression::setLHS(astNode* lhs) {
   _lhs = lhs;
   _lhsType = lhs ? lhs->nodeType() : NodeType::NONE;
}
void clef::Expression::setRHS(astNode* rhs) {
   _rhs = rhs;
   _rhsType = rhs ? rhs->nodeType() : NodeType::NONE;
}
void clef::Expression::setExtra(astNode* extra) {
   _extra = extra;
   _extraType = extra ? extra->nodeType() : NodeType::NONE;
}

#endif //EXPRESSION_CPP