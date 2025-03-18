#pragma once
#ifndef LITERAL_HPP
#define LITERAL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

#include "str_slice.hpp"
// #include "string.hpp"

struct clef::Literal {
   private:
      union {
         void* _ptrLit;
         ulong _uintLit;
         slong _sintLit;
         flong _floatLit;
         char _charLit;
         mcsl::str_slice _strLit;
         mcsl::str_slice _interpLit;
         mcsl::str_slice _formatLit;
         mcsl::str_slice _regexLit;
         index<Type> _typeid;
      };
      LitType _type;
   public:
      static constexpr NodeType nodeType() { return NodeType::LITERAL; }

      #pragma region constructors
      Literal(const char ch):_charLit{ch},_type{LitType::CHAR} {}
      template<mcsl::uint_t  uint_t > Literal(const  uint_t i):_uintLit{i}, _type{LitType::UINT}  {}
      template<mcsl::sint_t  sint_t > Literal(const  sint_t i):_sintLit{i}, _type{LitType::SINT}  {}
      template<mcsl::float_t float_t> Literal(const float_t i):_floatLit{i},_type{LitType::FLOAT} {}
      Literal(mcsl::str_slice str):_strLit{str},_type{LitType::STRING} {}
      Literal(mcsl::str_slice str, const LitType type):_strLit{str},_type{type} {
         //check type
         if (_type != LitType::STRING && _type != LitType::INTERP_STR && _type != LitType::FORMAT && _type != LitType::REGEX) {
            throwError(ErrCode::BAD_LITERAL, mcsl::FMT("attempt to construct string-like Literal node with non-string-like LitType"));
         }
      }
      Literal(index<Type> typeID):_typeid{typeID},_type{LitType::TYPEID} {}

      Literal(void* ptr):_ptrLit{ptr},_type{LitType::POINTER} {}

      Literal(Literal& other) {
         if (this != &other) {
            assert((ulong)absdif(this, &other) >= 8U*sizeof(Literal));
            std::memcpy(this, &other, sizeof(Literal));
         }
      }
      #pragma endregion constructors

      LitType type() const { return _type; }

      [[noreturn]] void throwCastErr(LitType target) const { throwError(ErrCode::BAD_LIT_CAST, mcsl::FMT("%u to %u"), +_type, +target); }

      #pragma region cast
      operator ulong()        const { if (_type == LitType::UINT)   { return _uintLit;   } else { throwCastErr(LitType::UINT);   } }
      operator slong()        const { if (_type == LitType::SINT)   { return _sintLit;   } else { throwCastErr(LitType::SINT);   } }
      operator flong()       const { if (_type == LitType::FLOAT)  { return _floatLit;  } else { throwCastErr(LitType::FLOAT);  } }
      operator char()         const { if (_type == LitType::CHAR)   { return _charLit;   } else { throwCastErr(LitType::CHAR);   } }
      operator mcsl::str_slice&()      { if (_type == LitType::STRING) { return _strLit;    } else { throwCastErr(LitType::STRING); } }
      operator const mcsl::str_slice&() const { if (_type == LitType::STRING) { return _strLit;    } else { throwCastErr(LitType::STRING); } }
      // operator mcsl::format() const { if (_type == LitType::FORMAT) { return _formatLit; } else { throwCastErr(LitType::FORMAT); } }
      // operator mcsl::regex()  const { if (_type == LitType::REGEX)  { return _regexLit;  } else { throwCastErr(LitType::REGEX);  } }
      operator index<Type>() { if(_type == LitType::TYPEID) { return _typeid; } else { throwCastErr(LitType::TYPEID); }}
      operator index<const Type>() const { if(_type == LitType::TYPEID) { return _typeid; } else { throwCastErr(LitType::TYPEID); }}
      operator void*() { if (_type == LitType::POINTER) { return _ptrLit; } else { throwCastErr(LitType::POINTER); }}
      operator const void*() const { if (_type == LitType::POINTER) { return _ptrLit; } else { throwCastErr(LitType::POINTER); }}
      #pragma endregion cast

      bool operator==(const Literal& other) const {
         if (_type != other._type) { return false; }
         switch(_type) {
            case LitType::NONE      : return true;
            case LitType::POINTER   : return    _ptrLit == other._ptrLit;
            case LitType::UINT      : return   _uintLit == other._uintLit;
            case LitType::SINT      : return   _sintLit == other._sintLit;
            case LitType::FLOAT     : return  _floatLit == other._floatLit;
            case LitType::CHAR      : return   _charLit == other._charLit;
            case LitType::STRING    : return    _strLit == other._strLit;
            case LitType::INTERP_STR: return _interpLit == other._interpLit;
            case LitType::FORMAT    : return _formatLit == other._formatLit;
            case LitType::REGEX     : return  _regexLit == other._regexLit;
            case LitType::TYPEID    : return    _typeid == other._typeid;
         }
      }

      void printf() const;
};

#endif //LITERAL_HPP