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
   template <uint _size> struct OpDecoder;
   struct DelimPair;
   struct KeywordSpec;
   class KeywordDecoder;
   // template <uint _size> struct KeywordDecoder;

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
            struct EnumUnion; //!TODO: implement enumunions
            struct Namespace;
            struct Interface;
            struct Struct;
            struct Class;
            struct GenericType;
      struct Scope;
      struct Literal;
      struct Expression; using Expr = Expression;
         struct Declaration; using Decl = Declaration;
         struct Statement; using Stmt = Statement;
         struct ForLoop;
            struct ForeachLoop;
            struct WhileLoop;
            struct DoWhileLoop;
            struct If;
            struct Switch;
            struct Match;
            struct TryCatch;
            struct Asm;
      struct ForLoopParams;
      struct ForeachLoopParams;
      struct SwitchCases;
      struct MatchCases;
      struct StatementSequence; using StmtSeq = StatementSequence;
      struct ArgumentList; using ArgList = ArgumentList;
      struct ParameterList; using ParamList = ParameterList;
      template<typename T> concept astNode_t = requires { {T::nodeType()} -> mcsl::same_t<NodeType>; };
      template<typename T> concept astNode_ptr_t = mcsl::ptr_t<T> && astNode_t<mcsl::remove_ptr<T>>;
      template<typename T> concept operand_t = mcsl::is_t<T, clef::Identifier> || mcsl::is_t<T,Expression>;
   class ObjTypeSpec;
   class InterfaceSpec;
   class NamespaceSpec;
   struct Symbol;

   //containers for atomic compilation elements
   class Source;
   class SourceTokens;
   class SymbolTable;
   class SyntaxTree;

   //classes for phases of compilation
   class Lexer;
   class Parser;
}

template<typename T> struct clef::index {
   uint i;

   index(const uint n = {}):i{n} {}

   operator uint&() { return i; }
   operator uint() const { return i; }

   uint operator+() const { return i; }

   operator index<const T>() const { return i; }

   template<typename parent_t> requires (mcsl::is_t<T, parent_t>) operator index<parent_t>() { return i; }
   template<typename parent_t> requires (mcsl::is_t<T, parent_t>) operator index<const parent_t>() const { return i; }
};

#endif //CLEF_HPP