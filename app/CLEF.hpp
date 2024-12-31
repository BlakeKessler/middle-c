#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "raw_str.hpp"

namespace clef {
   //utilities
   class allocator;
   template<typename T> struct index;

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
      struct Identifier;
         struct Variable;
         struct Function;
         struct Type;
            struct VariadicParameter; using VariadicParam = VariadicParameter;
            struct FundamentalType; using FundType = FundamentalType;
            struct FunctionSignature; using FuncSig = FunctionSignature;
            struct Enum;
            struct Mask;
            struct Union;
            struct Namespace;
            struct Interface;
            struct Struct;
            struct Class;
            struct GenericType;
      struct Scope;
      struct Literal;
      struct Expression; using Expr = Expression;
         struct Declaration; using Decl = Declaration;
         struct Loop;
         struct If;
         struct Else;
         struct ElseIf;
         struct Switch;
         struct Match;
         struct TryCatch;
         struct Asm;
      struct ForLoopParams;
      struct ForeachLoopParams;
      struct SwitchCases;
      struct MatchCases;
      struct Statement; using Stmt = Statement;
      struct StatementSequence; using StmtSeq = StatementSequence;
      struct ArgumentList; using ArgList = ArgumentList;
      struct ParameterList; using ParamList = ParameterList;
   class ObjTypeSpec;
   class InterfaceSpec;
   class NamespaceSpec;
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

template<typename T> struct clef::index{
   uint i;
   operator uint&() { return i; }
   operator uint() { return i; }
};

#endif //CLEF_HPP