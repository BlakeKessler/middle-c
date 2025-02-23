#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include <cstdio>

#define ID(fund) id = fund; break
//!NOTE: currently uses LP64 data model no matter what
clef::index<clef::FundType> clef::SyntaxTree::getFundType(const KeywordID keyword) {
   using enum FundTypeID;
   FundTypeID id;
   switch (keyword) {
      case KeywordID::VOID       : ID(VOID);
      case KeywordID::AUTO       : ID(AUTO);


      case KeywordID::CHAR       : ID(CHAR);
      case KeywordID::CHAR_UTF_8 : ID(CHAR8);
      case KeywordID::CHAR_UTF_16: ID(CHAR16);
      case KeywordID::CHAR_UTF_32: ID(CHAR32);


      case KeywordID::BOOL       : ID(BOOL);
      case KeywordID::UBYTE      : ID(UINT8);
      case KeywordID::USHORT     : ID(UINT16);
      case KeywordID::UINT       : ID(UINT32);
      case KeywordID::ULONG      : ID(UINT64);
      case KeywordID::UINT_PTR   : ID(UINT64);
      case KeywordID::UWORD      : ID(UINT64);

      case KeywordID::SIGN_T     : ID(SIGN_T);
      case KeywordID::SBYTE      : ID(SINT8);
      case KeywordID::SSHORT     : ID(SINT16);
      case KeywordID::SINT       : ID(SINT32);
      case KeywordID::SLONG      : ID(SINT64);
      case KeywordID::SINT_PTR   : ID(SINT64);
      case KeywordID::SWORD      : ID(SINT64);


      case KeywordID::UINT_8     : ID(UINT8);
      case KeywordID::UINT_16    : ID(UINT16);
      case KeywordID::UINT_32    : ID(UINT32);
      case KeywordID::UINT_64    : ID(UINT64);
      case KeywordID::UINT_128   : ID(UINT128);
      case KeywordID::UINT_256   : ID(UINT256);

      case KeywordID::SINT_8     : ID(SINT8);
      case KeywordID::SINT_16    : ID(SINT16);
      case KeywordID::SINT_32    : ID(SINT32);
      case KeywordID::SINT_64    : ID(SINT64);
      case KeywordID::SINT_128   : ID(SINT128);
      case KeywordID::SINT_256   : ID(SINT256);

      case KeywordID::FLOAT      : ID(FLOAT32);

      case KeywordID::FLOAT_16   : ID(FLOAT16);
      case KeywordID::FLOAT_32   : ID(FLOAT32);
      case KeywordID::FLOAT_64   : ID(FLOAT64);
      case KeywordID::FLOAT_128  : ID(FLOAT128);
      case KeywordID::FLOAT_256  : ID(FLOAT256);


      case KeywordID::ENUM       : ID(ENUM);
      case KeywordID::MASK       : ID(MASK);
      case KeywordID::UNION      : ID(UNION);
      case KeywordID::NAMESPACE  : ID(NAMESPACE);
      case KeywordID::INTERFACE  : ID(INTERFACE);
      case KeywordID::STRUCT     : ID(STRUCT);
      case KeywordID::CLASS      : ID(CLASS);

      default: UNREACHABLE;
   }
   return make<FundType>(id);
}
#undef ID

clef::index<clef::astNode> clef::SyntaxTree::getValueKeyword(const KeywordID keyword) {
   switch (keyword) {
      case KeywordID::THIS    : return +make<Identifier>(KeywordID::THIS);
      case KeywordID::SELF    : return +make<Identifier>(KeywordID::SELF);
      case KeywordID::TRUE    : return +make<Literal>(true);
      case KeywordID::FALSE   : return +make<Literal>(false);
      case KeywordID::NULLPTR : return +make<Literal>((void*)nullptr);

      default: UNREACHABLE;
   }
}

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> index) {
   astNode& node = self[index];
   switch (node.nodeType()) {
      case Identifier::nodeType():
      case Variable::nodeType():
      case Function::nodeType():
      case Type::nodeType():
      case VariadicParameter::nodeType():
      case FundamentalType::nodeType():
      case FunctionSignature::nodeType():
      case Enum::nodeType():
      case Mask::nodeType():
      case Union::nodeType():
      case Namespace::nodeType():
      case Interface::nodeType():
      case Struct::nodeType():
      case Class::nodeType():
      case GenericType::nodeType():
      case Literal::nodeType():
         return make<Expr>(op, node.nodeType(), index);

      case Expression::nodeType():
      case Declaration::nodeType():
      case Statement::nodeType():
      case ForLoop::nodeType():
      case ForeachLoop::nodeType():
      case WhileLoop::nodeType():
      case DoWhileLoop::nodeType():
      case If::nodeType():
      case Switch::nodeType():
      case Match::nodeType():
      case TryCatch::nodeType():
      case Asm::nodeType():
         return +index;


      case Scope::nodeType():
      case ForLoopParams::nodeType():
      case ForeachLoopParams::nodeType():
      case SwitchCases::nodeType():
      case MatchCases::nodeType():
      case StatementSequence::nodeType():
      case ArgumentList::nodeType():
      case ParameterList::nodeType():
      case SpecializerList::nodeType():
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}

clef::index<clef::Expr> clef::SyntaxTree::makeExpr(const OpID op, index<astNode> lhsI, index<astNode> rhsI) {
   return make<Expr>(op, self[lhsI].nodeType(), self[rhsI].nodeType(), lhsI, rhsI);
}


void clef::SyntaxTree::print() const {
   for (uint i = 1; i < _buf.size(); ++i) {
      std::printf("\n\t%u: ", i);
      _buf[i].print();
   }
   std::printf("\n");
}
void clef::SyntaxTree::printf() const {
   for (uint i = 1; i < _buf.size(); ++i) {
      std::printf("\n\t%u: ", i);
      _buf[i].printf();
   }
   std::printf("\n");
}

#endif //SYNTAX_TREE_CPP