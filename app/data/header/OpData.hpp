#pragma once
#ifndef OP_DATA_HPP
#define OP_DATA_HPP

#include "CLEF.hpp"
#include "TokenData.hpp"

#include "static_arr.hpp"
#include "arr_span.hpp"
#include "raw_str.hpp"

struct alignas(8) clef::OpData {
   private:
      mcsl::raw_str<MAX_OP_LEN> _opStr;
      byte _opStrLen;
      OpID _id;
      OpProps _props;
      
   public:
      constexpr OpData():_opStr{},_opStrLen{},_props{},_id{} {}
      template<mcsl::str_t str_t> constexpr OpData(const str_t& str, const OpID id, const OpProps props)
         :_opStr{str},_opStrLen{(byte)_opStr.strlen()},_id{id},_props{props} {}
      constexpr OpData(const char str[MAX_OP_LEN + 1], const OpID id, const OpProps props)
         :_opStr{str},_opStrLen{(byte)_opStr.strlen()},_id{id},_props{props} {}

      constexpr uint size() const { return _opStr.size(); }
      constexpr OpID id() const { return _id; }
      constexpr byte precedence() const { return +(OpProps::__PRECEDENCE_BITS & _props); }
      constexpr OpProps props() const { return _props; }
      constexpr auto toString() const { return _opStr; }

      constexpr bool combineWith(const OpData other); //merge other into self

      template<mcsl::str_t str_t> constexpr bool operator<=>(const str_t& str) { return _opStr <=> str; }
      template<mcsl::str_t str_t> constexpr bool operator==(const str_t& str) { return _opStr == str; }
      template<mcsl::str_t str_t> constexpr bool operator!=(const str_t& str) { return _opStr != str; }

      constexpr bool operator==(const OpData& other) const { return _id == other._id; }
      constexpr bool operator< (const OpData& other) const { return precedence() <  other.precedence(); }
      constexpr bool operator<=(const OpData& other) const { return precedence() <= other.precedence(); }
      constexpr bool operator> (const OpData& other) const { return precedence() >  other.precedence(); }
      constexpr bool operator>=(const OpData& other) const { return precedence() >= other.precedence(); }

      void printf() const { return _opStr.printf(); }
};

#endif //OP_DATA_HPP