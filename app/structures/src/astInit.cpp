#ifndef AST_INIT_CPP
#define AST_INIT_CPP

#include "SyntaxTree.hpp"
#include "DataModel.hpp"
#include "pretty-print.hpp"

void clef::SyntaxTree::initTables() {
   using enum FundTypeID;

   make<StmtSeq>(&allocBuf<index<Stmt>>());

   const auto init = [&](FundTypeID type) {
      mcsl::str_slice name = toString(type);
      TypeSpec* spec = _typeTable.emplace_back(type);
      SymbolNode* node = _symbolBuf.emplace_back(name, &_globalScope, spec, SymbolType::FUND_TYPE);
      spec->canonName() = node;
      _globalScope[name] = node;
      _fundTypes.insert(type, node);
   };
   
   init(VOID);
   init(AUTO);


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
   init(FLOAT80);
   init(FLOAT128);
   init(FLOAT256);
}

#endif //AST_INIT_CPP