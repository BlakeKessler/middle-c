#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "raw_str.hpp"

namespace clef {
   //objects for encoding Middle C specification data
   struct OpData;
   struct OpGroup;
   template <uint _size> struct OpDecoder;
   struct DelimPair;
   struct KeywordSpec;
   template <uint _size> struct KeywordDecoder;

   //atomic compilation elements
   struct Token;
   struct astNode;
      struct Expression;
      struct Statement;
      struct StmtSequence;
      struct Identifier;
      struct Block;
      struct Type;
      struct Literal;
      struct Object;
      struct Function;
      struct Operator;
      struct Template;
   struct Symbol;

   //containers for atomic compilation elements
   class Source;
   struct SourceTokens;
   struct SymbolTable;
   struct SyntaxTree;

   //classes for phases of compilation
   class Lexer;
   class Parser;
}

#endif //CLEF_HPP