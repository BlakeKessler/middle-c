#pragma once
#ifndef OP_DATA_HPP
#define OP_DATA_HPP

#include "CLEF.hpp"
#include "TokenData.hpp"

#include "static_arr.hpp"
#include "arr_span.hpp"
#include "raw_buf_str.hpp"

struct alignas(8) clef::OpData {
   private:
      mcsl::raw_buf_str<MAX_OP_LEN, ubyte> _opStr;
      OpID _id;
      OpProps _props;
      ubyte _prec;
      TokenType _tokType;
      
   public:
      constexpr OpData():_opStr{},_id{},_props{},_prec{},_tokType{} {}
      template<mcsl::str_t str_t> constexpr OpData(const str_t& str, const OpID id, const OpProps props, const ubyte prec, const TokenType tokType)
         :_opStr{str},_id{id},_props{props},_prec{prec},_tokType{tokType} {}
      constexpr OpData(const char str[MAX_OP_LEN + 1], const OpID id, const OpProps props, const ubyte prec, const TokenType tokType)
         :_opStr{str},_id{id},_props{props},_prec{prec},_tokType{tokType} {}

      constexpr ubyte size() const { return _opStr.size(); }
      constexpr OpID opID() const { return _id; }
      constexpr ubyte precedence() const { return _prec; }
      constexpr OpProps props() const { return _props; }
      constexpr TokenType tokType() const { return _tokType; }
      constexpr auto toString() const { return _opStr; }

      constexpr bool combineWith(const OpData other) { //merge other into self
         if (toString() != other.toString() || (precedence() && precedence() != other.precedence())) {
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

      template<mcsl::str_t str_t> constexpr bool operator<=>(const str_t& str) { return _opStr <=> str; }
      template<mcsl::str_t str_t> constexpr bool operator==(const str_t& str) { return _opStr == str; }
      template<mcsl::str_t str_t> constexpr bool operator!=(const str_t& str) { return _opStr != str; }

      constexpr bool operator==(const OpID other) const { return _id == other; }
      constexpr bool operator==(const OpData other) const { return _id == other._id; }
      constexpr bool operator< (const OpData other) const { return precedence() <  other.precedence(); }
      constexpr bool operator<=(const OpData other) const { return precedence() <= other.precedence(); }
      constexpr bool operator> (const OpData other) const { return precedence() >  other.precedence(); }
      constexpr bool operator>=(const OpData other) const { return precedence() >= other.precedence(); }

      constexpr operator bool() const { return +_id; }
      constexpr operator OpID() const { return _id; }
      constexpr operator OpProps() const { return _props; }

      void printf() const { return _opStr.printf(); }
};

#endif //OP_DATA_HPP