#ifndef AST_INIT_CPP
#define AST_INIT_CPP

#include "SyntaxTree.hpp"
#include "DataModel.hpp"
#include "pretty-print.hpp"

void clef::SyntaxTree::initTables() {
   using enum KeywordID;

   const auto init = [&](KeywordID type) {
      mcsl::str_slice name = toString(type);
      TypeSpec* spec = _typeTable.emplace_back(toTypeID(type, _dataModel));
      SymbolNode* node = _symbolBuf.emplace_back(name, &_globalScope, spec, SymbolType::FUND_TYPE);
      _globalScope[name] = node;
      _keywordTypes.insert(type, node);
   };
   
   init(VOID);
   init(AUTO);


   init(CHAR);
   init(CHAR_UTF_8);
   init(CHAR_UTF_16);
   init(CHAR_UTF_32);


   init(BOOL);
   init(UBYTE);
   init(USHORT);
   init(UINT);
   init(ULONG);
   init(UOVERLONG);
   init(UINT_PTR);
   init(UWORD);

   init(UINT_8);
   init(UINT_16);
   init(UINT_32);
   init(UINT_64);
   init(UINT_128);
   init(UINT_256);


   init(SIGN_T);
   init(SBYTE);
   init(SSHORT);
   init(SINT);
   init(SLONG);
   init(SOVERLONG);
   init(SINT_PTR);
   init(SWORD);

   init(SINT_8);
   init(SINT_16);
   init(SINT_32);
   init(SINT_64);
   init(SINT_128);
   init(SINT_256);

   init(FLOAT);
   init(FLONG);
   init(FLEXT);

   init(FLOAT_16);
   init(FLOAT_32);
   init(FLOAT_64);
   init(FLOAT_80);
   init(FLOAT_128);
   init(FLOAT_256);
}

#endif //AST_INIT_CPP