#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "throw.hpp"
#include "arr_list.hpp"

namespace clef {
   //utilities
   template<typename T> struct res;
   template<typename T> class Box;
   template<typename T> using buffer = mcsl::arr_list<T, ARR_LIST_BUF_SIZE>;

   //objects for encoding Middle C specification data
   struct OpData;
   template <uint _size> struct OpDecoder;
   struct OpPrecs;
   struct DelimPair;
   struct KeywordSpec;
   class KeywordDecoder;

   //atomic compilation elements
   struct Token;
   class Symbol;
   class TypeSpec;

   class Attr;

   struct Identifier;
   struct Label;
   struct Literal;
   class Args;
   class Proc;
   struct Expr;
   struct Ternary;
   struct If;
   struct While;
   struct DoWhile;
   struct For;
   struct Foreach;
   struct Switch;
   struct Match;
   class Func;
   class Method;
   class Macro;
   struct Overload;

   template<typename T> concept SyntaxNode_t = mcsl::same_t<typename T::SyntaxNode_t, void>;

   //containers for atomic compilation elements
   class Source;
   class SyntaxTree;

   //classes for phases of compilation
   class Lexer;
   class Parser;
   class CodeGenerator;
}

template<diff_t<void> T> struct clef::res<T> {
   private:
      union {
         T _ok;
         ErrCode _err;
      };
      bool _is_ok;

   public:
      res(T obj):_ok{obj},_is_ok{true} {}
      res(ErrCode e):_err{e},_is_ok{false} {}

      bool is_ok() { return _is_ok; }
      bool is_err() { return !_is_ok; }
      T ok() { assume(is_ok()); return _ok; }
      ErrCode err() { assume(is_err()); return _err; }

      T orelse(T other) { if (is_ok()) { return ok(); } else { return other; } }
      T expect(const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (is_ok()) {
            return _ok;
         } else {
            mcsl::__throw(fmt, std::forward<decltype(argv)>(argv)...);
         }
      }
};
template<> struct clef::res<void> {
   private:
      union {
         ErrCode _err;
      };
      bool _is_ok;

   public:
      res():_is_ok{true} {}
      res(ErrCode e):_err{e},_is_ok{false} {}

      bool is_ok() { return _is_ok; }
      bool is_err() { return !_is_ok; }
      void ok() { assume(is_ok()); }
      ErrCode err() { assume(is_err()); return _err; }

      void orelse() { return; }
      void expect(const mcsl::str_slice fmt, mcsl::Printable auto... argv) {
         if (is_ok()) {
            return;
         } else {
            mcsl::__throw(fmt, std::forward<decltype(argv)>(argv)...);
         }
      }
};

#endif //CLEF_HPP