#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"

clef::Symbol* clef::SyntaxTree::registerSymbol(const mcsl::str_slice name, Identifier parent, Symbol::Type t) {
   return _symbols.emplace_back(name, parent, t);
}
clef::Symbol* clef::SyntaxTree::registerSymbolAnon(Identifier parent, Symbol::Type t) {
   return _symbols.emplace_back(parent, t);
}

#endif