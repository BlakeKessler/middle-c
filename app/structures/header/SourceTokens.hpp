#pragma once
#ifndef SOURCE_TOKENS_HPP
#define SOURCE_TOKENS_HPP

#include "Token.hpp"
#include "Source.hpp"

#include "array.hpp"
#include "dyn_arr.hpp"
#include "dyn_arr_span.hpp"

class clef::SourceTokens {
   protected:
      Source _src;

      mcsl::dyn_arr<Token> _tokens;
      mcsl::dyn_arr<char> _strLitBuf;
   public:
      //constructors
      SourceTokens():_src{},_tokens{},_strLitBuf{} {}
      SourceTokens(Source&& src):_src{std::move(src)},_tokens{},_strLitBuf{} {}
      void release() { _src.release(); _tokens.release(); _strLitBuf.release(); }

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

      mcsl::dyn_arr<Token>& tokens() { return _tokens; }
      const mcsl::dyn_arr<Token>& tokens() const { return _tokens; }
      Source& source() { return _src; }
      const Source& source() const { return _src; }
};

namespace mcsl {
   File& write(File&, const clef::SourceTokens&);
   uint writef(File&, const clef::SourceTokens&, char mode, FmtArgs args);
};

#endif //SOURCE_TOKENS_HPP