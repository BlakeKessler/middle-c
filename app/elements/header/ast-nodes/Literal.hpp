#pragma once
#ifndef LITERAL_HPP
#define LITERAL_HPP

#include "CLEF.hpp"

#include "str_slice.hpp"

struct [[clang::trivial_abi]] clef::Literal {
   private:
      union {
         void* _ptrLit;
         ulong _uintLit;
         slong _sintLit;
         flong _floatLit;
         bool _boolLit;
         char _charLit;
         mcsl::str_slice _strLit;
         mcsl::str_slice _interpLit;
         mcsl::str_slice _formatLit;
         mcsl::str_slice _regexLit;
         TypeSpec* _typeid;
      };
      FundTypeID _typeName;
      LitType _type;
   public:
      static constexpr NodeType nodeType() { return NodeType::LITERAL; }

      #pragma region constructors
      Literal(const char ch, FundTypeID t):_charLit{ch},_typeName(t),_type{LitType::CHAR}  { debug_assert(isChar(t)); }
      Literal(const ulong i, FundTypeID t):_uintLit{i}, _typeName(t),_type{LitType::UINT}  { debug_assert(isUint(t)); }
      Literal(const slong i, FundTypeID t):_sintLit{i}, _typeName(t),_type{LitType::SINT}  { debug_assert(isSint(t)); }
      Literal(const flong i, FundTypeID t):_floatLit{i},_typeName(t),_type{LitType::FLOAT} { debug_assert(isFloat(t)); }
      Literal(const bool b):_boolLit{b},_typeName(FundTypeID::BOOL),_type{LitType::BOOL} {}
      Literal(mcsl::str_slice str, FundTypeID t):_strLit{str},_typeName{t},_type{LitType::STRING} { debug_assert(isChar(t)); }
      Literal(mcsl::str_slice str, FundTypeID t, const LitType type):_strLit{str},_typeName{t},_type{type} {
         debug_assert(isChar(t));
         //check type
         if (_type != LitType::STRING && _type != LitType::INTERP_STR && _type != LitType::FORMAT && _type != LitType::REGEX) {
            internalError(ErrCode::BAD_LITERAL, mcsl::FMT("attempt to construct string-like Literal node with non-string-like LitType"));
         }
      }
      Literal(TypeSpec* typeID):_typeid{typeID},_typeName{FundTypeID::null},_type{LitType::TYPEID} {}

      Literal(void* ptr):_ptrLit{ptr},_typeName{FundTypeID::null},_type{LitType::POINTER} {}

      Literal(Literal& other) {
         std::memcpy((void*)this, &other, sizeof(Literal));
      }
      Literal(Literal&& other) {
         std::memcpy((void*)this, &other, sizeof(Literal));
      }
      #pragma endregion constructors

      LitType type() const { return _type; }
      FundTypeID typeName() const { return _typeName; }

      [[noreturn]] void throwCastErr(LitType target) const { internalError(ErrCode::BAD_LIT_CAST, mcsl::FMT("%u to %u"), +_type, +target); }

      #pragma region cast
      operator ulong()        const { if (_type == LitType::UINT)   { return _uintLit;   } else { throwCastErr(LitType::UINT);   } }
      operator slong()        const { if (_type == LitType::SINT)   { return _sintLit;   } else { throwCastErr(LitType::SINT);   } }
      operator flong()        const { if (_type == LitType::FLOAT)  { return _floatLit;  } else { throwCastErr(LitType::FLOAT);  } }
      operator bool()         const { if (_type == LitType::BOOL)   { return _boolLit;   } else { throwCastErr(LitType::BOOL);   } }
      operator char()         const { if (_type == LitType::CHAR)   { return _charLit;   } else { throwCastErr(LitType::CHAR);   } }
      operator mcsl::str_slice&()      { if (_type == LitType::STRING) { return _strLit;    } else { throwCastErr(LitType::STRING); } }
      operator const mcsl::str_slice() const { if (_type == LitType::STRING) { return _strLit;    } else { throwCastErr(LitType::STRING); } }
      // operator mcsl::format() const { if (_type == LitType::FORMAT) { return _formatLit; } else { throwCastErr(LitType::FORMAT); } }
      // operator mcsl::regex()  const { if (_type == LitType::REGEX)  { return _regexLit;  } else { throwCastErr(LitType::REGEX);  } }
      operator TypeSpec*() { if(_type == LitType::TYPEID) { return _typeid; } else { throwCastErr(LitType::TYPEID); }}
      operator const TypeSpec*() const { if(_type == LitType::TYPEID) { return _typeid; } else { throwCastErr(LitType::TYPEID); }}
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
            case LitType::BOOL      : return   _boolLit == other._boolLit;
            case LitType::CHAR      : return   _charLit == other._charLit;
            case LitType::STRING    : return    _strLit == other._strLit;
            case LitType::INTERP_STR: return _interpLit == other._interpLit;
            case LitType::FORMAT    : return _formatLit == other._formatLit;
            case LitType::REGEX     : return  _regexLit == other._regexLit;
            case LitType::TYPEID    : return    _typeid == other._typeid;
         }
      }
};

namespace mcsl {
   File& write(File&, const clef::Literal&);
};

#endif //LITERAL_HPP