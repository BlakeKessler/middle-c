#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

#include <cstdio>

#define ID(fund) id = fund; break
//!NOTE: currently uses LP64 data model no matter what
clef::FundType* clef::SyntaxTree::getFundType(const KeywordID keyword) {
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
      case KeywordID::UPTR       : ID(UINT64);
      case KeywordID::UWORD      : ID(UINT64);

      case KeywordID::SIGN_T     : ID(SIGN_T);
      case KeywordID::SBYTE      : ID(SINT8);
      case KeywordID::SSHORT     : ID(SINT16);
      case KeywordID::SINT       : ID(SINT32);
      case KeywordID::SLONG      : ID(SINT64);
      case KeywordID::SPTR       : ID(SINT64);
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

      default: UNREACHABLE;
   }
   return new (allocNode(NodeType::FUND_TYPE)) FundType{id};
}
#undef ID

clef::astNode* clef::SyntaxTree::getValueKeyword(const KeywordID keyword) {
   switch (keyword) {
      case KeywordID::THIS    : return (astNode*)(new (allocNode(NodeType::IDEN)) Identifier{KeywordID::THIS});
      case KeywordID::SELF    : return (astNode*)(new (allocNode(NodeType::IDEN)) Identifier{KeywordID::SELF});
      case KeywordID::TRUE    : return (astNode*)(new (allocNode(NodeType::LITERAL)) Literal{true});
      case KeywordID::FALSE   : return (astNode*)(new (allocNode(NodeType::LITERAL)) Literal{false});
      case KeywordID::NULLPTR : return (astNode*)(new (allocNode(NodeType::LITERAL)) Literal{(void*)nullptr});

      default: UNREACHABLE;
   }
}


void clef::SyntaxTree::print() const {
   std::printf("ROOT: index = %ld, addr = %p", _buf.begin() - _root, (void*)_root);
   for (uint i = 0; i < _buf.size(); ++i) {
      std::printf("\n\t");
      _buf[i].print();
   }
   std::printf("\n");
}

void clef::SyntaxTree::printf() const {
   std::printf("HEY YOU! PROGRAMMING THIS! MAKE THIS ACTUALLY FORMATTED!\n");
   print();
}

#endif //SYNTAX_TREE_CPP