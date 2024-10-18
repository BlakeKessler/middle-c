#pragma once
#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "CLEF.hpp"

struct clef::Symbol {
   public:
      union {
         SymbolData* node;
         SymbolTable* table;
      };
      enum : byte {
         NIL = 0,
         NODE,
         TABLE
      } type;

      Symbol(SymbolData& obj): node(&obj), type(NODE) {}
      Symbol(SymbolData* obj): node(obj),  type(NODE) {}
      Symbol(SymbolTable& obj): table(&obj), type(TABLE) {}
      Symbol(SymbolTable* obj): table(obj),  type(TABLE) {}
};

#endif //SYMBOL_HPP