#ifndef AST_INIT_CPP
#define AST_INIT_CPP

#include "SyntaxTree.hpp"

void clef::SyntaxTree::initTables() {
   using enum FundTypeID;
   #define init(type) _globalScope[toString(type)] = _symbolBuf.emplace_back(toString(type), &_globalScope, _typeTable.emplace_back(type), SymbolType::FUND_TYPE)
   
   init(VOID);

   init(CHAR);
   init(CHAR8);
   init(CHAR16);
   init(CHAR32);

   init(BOOL);
   init(UINT8);
   init(UINT16);
   init(UINT32);
   init(UINT64);
   init(UINT128);
   init(UINT256);

   init(SIGN_T);
   init(SINT8);
   init(SINT16);
   init(SINT32);
   init(SINT64);
   init(SINT128);
   init(SINT256);

   init(FLOAT16);
   init(FLOAT32);
   init(FLOAT64);
   init(FLOAT128);
   init(FLOAT256);
   
   #undef init
}

#endif //AST_INIT_CPP