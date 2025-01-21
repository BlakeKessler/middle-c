#ifndef EXPRESSION_CPP
#define EXPRESSION_CPP

#include "ast-nodes/Expression.hpp"
#include "astNode.hpp"

clef::Expression::Expression(OpID op, astNode* lhs, astNode* rhs):
   _op{op},
   _lhsType{lhs?lhs->nodeType():NodeType::NONE},
   _rhsType{rhs?rhs->nodeType():NodeType::NONE},
   _extraType{NodeType::NONE},
   _lhs{lhs},_rhs{rhs},_extra{} {

}
clef::Expression::Expression(OpID op, Type* type, ArgList* ctorArgs):
   _op{op},
   _lhsType{NodeType::TYPE},
   _rhsType{NodeType::ARG_LIST},
   _extraType{NodeType::NONE},
   _lhs{type},_rhs{ctorArgs},_extra{} {
      debug_assert(op == OpID::CALL_INVOKE || op == OpID::LIST_INVOKE);
}
clef::Expression::Expression(OpID op, astNode* lhs, astNode* rhs, astNode* extra):
   _op{op},
   _lhsType{lhs?lhs->nodeType():NodeType::NONE},
   _rhsType{rhs?rhs->nodeType():NodeType::NONE},
   _extraType{extra?extra->nodeType():NodeType::NONE},
   _lhs{lhs},_rhs{rhs},_extra{extra} {

}

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