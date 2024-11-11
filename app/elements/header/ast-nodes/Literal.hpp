#pragma once
#ifndef LITERAL_HPP
#define LITERAL_HPP

#include "CLEF.hpp"

#include "string.hpp"

struct clef::Literal {
   private:
      union {
         ulong _intLit;
         double _floatLit;
         char _charLit;
         mcsl::string _strLit;
         mcsl::string _formatLit;
         mcsl::string _regexLit;
      };
      LitType _type;
   public:
      Literal(const ulong i):_intLit{i},_type{LitType::INT} {}
      Literal(const double i):_floatLit{i},_type{LitType::FLOAT} {}
      template<mcsl::str_t str_t> Literal(const str_t& str):_strLit{str},_type{LitType::STRING} {}
      template<mcsl::str_t str_t> Literal(const str_t& str, const LitType type):_strLit{str},_type{type} {
         //check type
         if (_type != LitType::STRING && _type != LitType::FORMAT && _type != LitType::REGEX) {
            throwError(ErrCode::BAD_LITERAL, "attempt to construct string-like Literal node with non-string-like LitType");
         }
      }

      bool operator==(const Literal& other) const {
         if (_type != other._type) { return false; }
         switch(_type) {
            case LitType::NONE  : return true;
            case LitType::INT   : return    _intLit == other._intLit;
            case LitType::FLOAT : return  _floatLit == other._floatLit;
            case LitType::CHAR  : return   _charLit == other._charLit;
            case LitType::STRING: return    _strLit == other._strLit;
            case LitType::FORMAT: return _formatLit == other._formatLit;
            case LitType::REGEX : return  _regexLit == other._regexLit;
         }
      }
};

#endif //LITERAL_HPP