#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "throw.hpp"

namespace clef {
   //utilities
   class allocator;
   template<typename T> struct index;

   //objects for encoding Middle C specification data
   struct OpData;
   template <uint _size> struct OpDecoder;
   struct OpPrecs;
   struct DelimPair;
   struct KeywordSpec;
   class KeywordDecoder;

   //atomic compilation elements
   struct Token;
   struct astNode;
      struct Identifier;
         struct Variable;
         struct Function;
         struct Macro;
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
            struct TypeDeclaration; using TypeDecl = TypeDeclaration;
         struct ForLoop;
            struct ForeachLoop;
            struct WhileLoop;
            struct DoWhileLoop;
            struct If;
            struct Switch;
            struct Match;
            struct TryCatch;
            struct Asm; //!TODO: implement asm
      struct ForLoopParams;
      struct ForeachLoopParams;
      struct SwitchCases;
      struct MatchCases;
      struct StatementSequence; using StmtSeq = StatementSequence;
      struct ArgumentList; using ArgList = ArgumentList;
      struct ParameterList; using ParamList = ParameterList;
      struct SpecializerList; using SpecList = SpecializerList;
      template<typename T> concept astNode_t = requires { {T::nodeType()} -> mcsl::same_t<NodeType>; } && !requires { T::IsPseudoNode; };
      template<typename T> concept astNode_ptr_t = mcsl::ptr_t<T> && astNode_t<mcsl::remove_ptr<T>>;
      template<typename T> concept pseudoNode_t = requires { {T::nodeType()} -> mcsl::same_t<NodeType>; };
      template<typename T> concept pseudoNode_ptr_t = mcsl::ptr_t<T> && pseudoNode_t<mcsl::remove_ptr<T>>;
      template<typename T> concept operand_t = mcsl::is_t<T, clef::Identifier> || mcsl::is_t<T,Expression>;
      #define CLEF_ALL_FUND_AST_NODE_T Expression, Function, Macro, Literal, Scope, Type, ForLoopParams, ForeachLoopParams, Identifier, Statement, Variable
      #define CLEF_ALL_EXPR_AST_NODE_T Declaration, TypeDecl, If, ForLoop, ForeachLoop, WhileLoop, DoWhileLoop, Switch, Match, TryCatch, Asm
      #define CLEF_ALL_NODE_LIST_AST_NODE_T ArgumentList, MatchCases, ParameterList, SpecializerList, StatementSequence, SwitchCases
      #define CLEF_ALL_TYPE_AST_NODE_T Class, Enum, Mask, FunctionSignature, FundamentalType, GenericType, Interface, Namespace, Struct, Union, VariadicParameter
      #define CLEF_ALL_AST_NODE_T CLEF_ALL_FUND_AST_NODE_T, CLEF_ALL_EXPR_AST_NODE_T, CLEF_ALL_NODE_LIST_AST_NODE_T, CLEF_ALL_TYPE_AST_NODE_T
   class ObjTypeSpec;
   class InterfaceSpec;
   class NamespaceSpec;
   #define CLEF_ALL_PSEUDO_NODE_T ObjTypeSpec, InterfaceSpec, NamespaceSpec
   class TypeDef;

   //containers for atomic compilation elements
   class Source;
   class SourceTokens;
   class SymbolNode;
   class IndirTable;
   class SyntaxTree;
      template<typename T> struct astTNB; //astTreeNodeBundle
      struct indenter;

   //classes for phases of compilation
   class Lexer;
   class Parser;
}

template<typename T> struct clef::index {
   using Type = T;

   uint i;

   index(const uint n = {}):i{n} {}

   operator uint&() { return i; }
   operator uint() const { return i; }

   uint operator+() const { return i; }

   operator index<const T>() const { return i; }
   index<const T> toConst() const { return i; }

   template<typename parent_t> requires (mcsl::is_t<T, parent_t>) operator index<parent_t>() { return i; }
   template<typename parent_t> requires (mcsl::is_t<T, parent_t>) operator index<const parent_t>() const { return i; }
};

#endif //CLEF_HPP