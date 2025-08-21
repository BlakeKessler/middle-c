#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

clef::Symbol* clef::SyntaxTree::registerSymbol(const mcsl::str_slice name, Identifier parent, Symbol::Type t) {
   return _symbols.emplace_back(name, parent, t);
}
clef::Symbol* clef::SyntaxTree::registerSymbolAnon(Identifier parent, Symbol::Type t) {
   return _symbols.emplace_back(parent, t);
}

clef::TypeSpec* clef::SyntaxTree::registerType(Symbol* symbol, TypeSpec::Metatype t) {
   debug_assert(!symbol->type());
   debug_assert(Symbol::isType(symbol->symbolType()));
   TypeSpec* spec = _types.emplace_back(symbol, t);
   symbol->setType({spec, symbol->genParams()});
   return spec;
}

clef::Identifier clef::SyntaxTree::getFundType(FundTypeID id) {
   using enum FundTypeID;
   static constexpr uint offsets[] {
      [+NULLPTR_T] =  0,
      [      +STR] =  1,

      [     +VOID] =  2,
      [     +AUTO] =  3,


      [     +CHAR] =  4,
      [    +CHAR8] =  5,
      [   +CHAR16] =  6,
      [   +CHAR32] =  7,

      [     +BOOL] =  8,
      [    +UINT8] =  9,
      [   +UINT16] = 10,
      [   +UINT32] = 11,
      [   +UINT64] = 12,
      [  +UINT128] = 13,
      [  +UINT256] = 14,

      [   +SIGN_T] = 15,
      [    +SINT8] = 16,
      [   +SINT16] = 17,
      [   +SINT32] = 18,
      [   +SINT64] = 19,
      [  +SINT128] = 20,
      [  +SINT256] = 21,

      [  +FLOAT16] = 22,
      [  +FLOAT32] = 23,
      [  +FLOAT64] = 24,
      [  +FLOAT80] = 25,
      [ +FLOAT128] = 26,
      [ +FLOAT256] = 27,
   };

   Identifier name{
      .symbol = (id == null ?
         nullptr :
         &_symbols[offsets[+id]]
      ),
      .gens = nullptr
   };
   //check that fundamental types are in the expected order
   if (id == null) {
      debug_assert(!name.symbol);
   } else {
      debug_assert(name.symbol);
      debug_assert(name.symbol->type().spec()->metaType() == TypeSpec::FUND);
      debug_assert(name.symbol->type().spec()->fund().id == id);
   }
   return name;
}

#endif