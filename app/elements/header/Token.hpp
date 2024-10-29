#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "CLEF.hpp"
#include "MidC_Data.hpp"
#include "raw_str_span.hpp"

struct clef::Token : public mcsl::raw_str_span {
   private:
      const TokenType _type;

      static constexpr const mcsl::raw_str _nameof = "Token";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      //constructors
      Token();
      Token(char* front, char* back, const TokenType type = TokenType::NONE): mcsl::raw_str_span(front, back - front),_type(type) {}
      Token(char* front, const uint length, const TokenType type = TokenType::NONE): mcsl::raw_str_span(front, length),_type(type) {}
      Token(Token&& other): mcsl::raw_str_span(std::move(other)),_type(other._type) {}

      // Token split(const uint len);

      //getters
      TokenType type() const { return _type; }

      //!check if a token represents a block delimiter
      inline DelimPairType blockDelimEval() const { return begin() ? blockDelimType(self) : DelimPairType::NONE; }
      //!check if a token represents a MiddleC keyword
      inline bool isKeyword() const { return begin() ? clef::isKeyword(self) : false; }

      //token type number calculation
      bool isType(const TokenType t) const { return +(_type & t); }

      void throwError(const ErrCode code) const { clef::throwError(code, "token: \033[4m%.*s\033[24m", size(), begin()); }
};

#endif //TOKEN_HPP