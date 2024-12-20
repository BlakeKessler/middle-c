#pragma once
#ifndef LITERAL_HPP
#define LITERAL_HPP

#include "CLEF.hpp"

#include "string.hpp"

struct clef::Literal {
   private:
      union {
         ulong _uintLit;
         slong _sintLit;
         double _floatLit;
         char _charLit;
         mcsl::string _strLit;
         mcsl::string _formatLit;
         mcsl::string _regexLit;
      };
      LitType _type;
   public:
      #pragma region constructors
      Literal(const char ch):_charLit{ch},_type{LitType::CHAR} {}
      template<mcsl::uint_t  uint_t > Literal(const  uint_t i):_uintLit{i}, _type{LitType::UINT}  {}
      template<mcsl::sint_t  sint_t > Literal(const  sint_t i):_sintLit{i}, _type{LitType::SINT}  {}
      template<mcsl::float_t float_t> Literal(const float_t i):_floatLit{i},_type{LitType::FLOAT} {}
      Literal(mcsl::string&& str):_strLit{str},_type{LitType::STRING} {}
      Literal(mcsl::string&& str, const LitType type):_strLit{str},_type{type} {
         //check type
         if (_type != LitType::STRING && _type != LitType::FORMAT && _type != LitType::REGEX) {
            throwError(ErrCode::BAD_LITERAL, "attempt to construct string-like Literal node with non-string-like LitType");
         }
      }
      template<mcsl::str_t str_t> Literal(const str_t& str):_strLit{str},_type{LitType::STRING} {}
      template<mcsl::str_t str_t> Literal(const str_t& str, const LitType type):_strLit{str},_type{type} {
         //check type
         if (_type != LitType::STRING && _type != LitType::FORMAT && _type != LitType::REGEX) {
            throwError(ErrCode::BAD_LITERAL, "attempt to construct string-like Literal node with non-string-like LitType");
         }
      }

      Literal(Literal&& other) {
         std::memmove(this, &other, sizeof(Literal));
         assert(this != &other);
         std::memset(&other, 0, sizeof(Literal));
      }

      ~Literal() {
         switch(_type) {
            case LitType::STRING: _strLit.~string();     break;
            case LitType::FORMAT: _formatLit.~string();  break;
            case LitType::REGEX : _regexLit.~string();   break;
            default:                                     break;
         }
      }
      #pragma endregion constructors

      [[noreturn]] void throwCastErr(LitType target) const { throwError(ErrCode::BAD_LIT_CAST, "%u to %u", (uint)_type, (uint)target); }

      #pragma region cast
      operator ulong()        const { if (_type == LitType::UINT)   { return _uintLit;   } else { throwCastErr(LitType::UINT);   } }
      operator slong()        const { if (_type == LitType::SINT)   { return _sintLit;   } else { throwCastErr(LitType::SINT);   } }
      operator double()       const { if (_type == LitType::FLOAT)  { return _floatLit;  } else { throwCastErr(LitType::FLOAT);  } }
      operator char()         const { if (_type == LitType::CHAR)   { return _charLit;   } else { throwCastErr(LitType::CHAR);   } }
      operator mcsl::string&()      { if (_type == LitType::STRING) { return _strLit;    } else { throwCastErr(LitType::STRING); } }
      operator const mcsl::string&() const { if (_type == LitType::STRING) { return _strLit;    } else { throwCastErr(LitType::STRING); } }
      // operator mcsl::format() const { if (_type == LitType::FORMAT) { return _formatLit; } else { throwCastErr(LitType::FORMAT); } }
      // operator mcsl::regex()  const { if (_type == LitType::REGEX)  { return _regexLit;  } else { throwCastErr(LitType::REGEX);  } }
      #pragma endregion cast

      bool operator==(const Literal& other) const {
         if (_type != other._type) { return false; }
         switch(_type) {
            case LitType::NONE  : return true;
            case LitType::UINT  : return   _uintLit == other._uintLit;
            case LitType::SINT  : return   _sintLit == other._sintLit;
            case LitType::FLOAT : return  _floatLit == other._floatLit;
            case LitType::CHAR  : return   _charLit == other._charLit;
            case LitType::STRING: return    _strLit == other._strLit;
            case LitType::FORMAT: return _formatLit == other._formatLit;
            case LitType::REGEX : return  _regexLit == other._regexLit;
         }
      }
};

#endif //LITERAL_HPP