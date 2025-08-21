#ifndef SYMBOL_CPP
#define SYMBOL_CPP

#include "Symbol.hpp"

clef::Symbol::Symbol(const mcsl::str_slice name, Identifier parent, Type t):
   _name(name),
   _gens(nullptr),
   _parent(parent),
   _children(),
   _type{},
   _symbolType(t) {
      
}
clef::Symbol::Symbol(Identifier parent, Type t):
   _name(),
   _gens(nullptr),
   _parent(parent),
   _children(),
   _type{},
   _symbolType(t) {
      
}

clef::res<clef::Symbol*> clef::Symbol::insert(Symbol* symbol) {
   if (symbol->symbolType() == LABELED_SCOPE) {
      TODO;
   }
   else if (symbol->isAnon()) {
      _children.anon.push_back(symbol);
      return symbol;
   } else {
      bool didInsert = _children.named.insert(symbol->name(), symbol);
      if (didInsert) {
         return symbol;
      } else {
         return ErrCode::CONFLICTING_REDECL;
      }
   }
}
clef::res<clef::Symbol*> clef::Symbol::get(const mcsl::str_slice name) {
   Symbol** sptr = _children.named.find(name);
   if (sptr) {
      return *sptr;
   }
   auto r = __ANON_CHILDREN_SEARCH(name);
   if (r.is_ok()) {
      return r;
   }
   if (_parent) {
      [[clang::musttail]] return _parent.symbol->get(name);
   }
   return {ErrCode::SYMBOL_NOT_FOUND};
}
clef::res<clef::Symbol*> clef::Symbol::__ANON_CHILDREN_SEARCH(const mcsl::str_slice name) {
   for (Symbol* anonChild : _children.anon) {
      Symbol** sptr = _children.named.find(name);
      if (sptr) {
         return *sptr;
      }
      auto r = anonChild->__ANON_CHILDREN_SEARCH(name);
      if (r.is_ok()) {
         return r;
      }
   }
   return {ErrCode::SYMBOL_NOT_FOUND};
}

#endif