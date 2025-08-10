#pragma once
#ifndef OP_DATA_HPP
#define OP_DATA_HPP

#include "CLEF.hpp"
#include "TokenData.hpp"

#include "static_arr.hpp"
#include "arr_span.hpp"
#include "raw_buf_str.hpp"

#include "io.hpp"

struct alignas(8) clef::OpData {
   private:
      mcsl::raw_buf_str<MAX_OP_LEN, ubyte> _opStr;
      Oplike _op;
      OpProps _props;
      ubyte _prec;
      TokenType _tokType;
      
   public:
      constexpr OpData():_opStr{},_op{},_props{},_prec{},_tokType{} {}
      template<mcsl::str_t str_t> constexpr OpData(const str_t& str, const Oplike op, const OpProps props, const ubyte prec, const TokenType tokType)
         :_opStr{str},_op{op},_props{props},_prec{prec},_tokType{tokType} {}

      constexpr ubyte size() const { return _opStr.size(); }
      constexpr Oplike op() const { return _op; }
      constexpr OpID opID() const { return toOpID(_op); }
      constexpr ubyte precedence() const { return _prec; }
      constexpr OpProps props() const { return _props; }
      constexpr TokenType tokType() const { return _tokType; }
      constexpr auto toString() const { return _opStr; }

      constexpr bool combineWith(const OpData other) { //merge other into self
         if (toString() != other.toString() || (precedence() && other.precedence() && precedence() != other.precedence())) {
            return false;
         }
         _props = _props | other.props();
         _prec = _prec ? _prec : other._prec;
         return true;
      }
      constexpr OpProps removeProps(const OpProps props) {
         _props = _props & ~props;
         return _props;
      }
      constexpr void setProps(const OpProps props) {
         _props = props;
      }
      constexpr void setPrecedence(ubyte prec) {
         _prec = prec;
      }

      constexpr bool operator<=>(const mcsl::str_slice str) { return _opStr <=> str; }
      constexpr bool operator==(const mcsl::str_slice str) { return _opStr == str; }
      constexpr bool operator!=(const mcsl::str_slice str) { return _opStr != str; }

      constexpr bool operator==(const Oplike other) const { return _op == other; }
      constexpr bool operator==(const OpData other) const { return _op == other._op; }
      constexpr bool operator< (const OpData other) const { return precedence() <  other.precedence(); }
      constexpr bool operator<=(const OpData other) const { return precedence() <= other.precedence(); }
      constexpr bool operator> (const OpData other) const { return precedence() >  other.precedence(); }
      constexpr bool operator>=(const OpData other) const { return precedence() >= other.precedence(); }

      explicit constexpr operator bool() const { return +_op; }
};

#endif //OP_DATA_HPP