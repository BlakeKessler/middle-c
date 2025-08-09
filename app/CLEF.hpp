#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "throw.hpp"

namespace clef {
   //utilities
   template<typename T> struct res;
   template<typename T> class Box;

   //objects for encoding Middle C specification data
   struct OpData;
   template <uint _size> struct OpDecoder;
   struct OpPrecs;
   struct DelimPair;
   struct KeywordSpec;
   class KeywordDecoder;

   //atomic compilation elements
   struct Token;

   //containers for atomic compilation elements
   class Source;

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
};

#endif //CLEF_HPP