#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "raw_str.hpp"

namespace clef {
   //objects for encoding Middle C specification data
   struct Operator;
   struct DelimPair;

   //atomic compilation elements
   struct Token;
   union astNode;
      struct Block;
      struct Comment;
      struct Declaration;
      struct Definition;
      struct Identifier;
      struct Literal;
      struct Object;
      struct Scope;
      struct Operator;
      struct Statement;
      struct Type;
      struct TypeQualifier;
   struct Symbol;
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