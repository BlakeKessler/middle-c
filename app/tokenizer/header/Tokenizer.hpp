#pragma once
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "Token.hpp"
#include "Source.hpp"

#include "array.hpp"
#include "dyn_arr.hpp"
#include "dyn_arr_span.hpp"

class clef::Tokenizer {
   protected:
      Source _src;
      mcs::dyn_arr<Token> _tokens;
      
      mcs::array<mcs::dyn_arr_span<Token>> _tokLines;

   public:
      //constructors
      Tokenizer();
      Tokenizer(Source& src);
      ~Tokenizer() { this->free(); }
      void free() const { _tokens.free();}

      static Tokenizer TokenizeFile(const char* path);

      //properties
      uint size() const { return _tokens.size(); }

      //element access
      const Token& operator[](const uint i) const { return _tokens[i]; }
      const Token& at(const uint i) const { return _tokens.at(i); }
      Token& operator[](const uint i) { return _tokens[i]; }
      Token& at(const uint i) { return _tokens.at(i); }

      const Token& front() const { return _tokens.front(); }
      const Token& back() const { return _tokens.back(); }

      const Token* begin() const { return _tokens.begin(); }
      const Token* end() const { return _tokens.end(); }
      Token* begin() { return _tokens.begin(); }
      Token* end() { return _tokens.end(); }

      mcs::dyn_arr_span<Token>& row(const uint i) const;
      const Source& source() const { return _src; }
      Source& source() { return _src; }
      const auto& lineArr() const { return _tokLines; }
      auto& lineArr() { return _tokLines; }

      void printf() const;
};

namespace clef {
   static Tokenizer (*TokenizeFile)(const char* path) = &Tokenizer::TokenizeFile;
}

#endif //TOKENIZER_HPP