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
      byte _precedence;
      OpType _type;
      byte _opID;
   public:
      constexpr OpData():_opStr{},_opStrLen{},_precedence{},_type{},_opID{} {}
      template<mcsl::str_t str_t> constexpr OpData(const str_t& str, const byte precedence, const OpType type, const byte id = 0)
         :_opStr{str},_opStrLen{str.strlen()},_precedence{precedence},_type{type},_opID{id} {}
      constexpr OpData(const char str[MAX_OP_LEN + 1], const byte precedence, const OpType type, const byte id = 0)
         :_opStr{str},_opStrLen{(byte)_opStr.strlen()},_precedence{precedence},_type{type},_opID{id} { _opStrLen = _opStrLen > _opStr.size() ? _opStr.size() : _opStrLen; }

      constexpr uint size() const { return _opStr.size(); }
      constexpr byte id() const { return _opID; }
      constexpr byte precedence() const { return _precedence; }
      constexpr OpType type() const { return _type; }
      constexpr auto toString() const { return _opStr; }

      constexpr void resetID(const byte id) { _opID = id; }

      template<mcsl::str_t str_t> constexpr bool operator<=>(const str_t& str) { return _opStr <=> str; }
      template<mcsl::str_t str_t> constexpr bool operator==(const str_t& str) { return _opStr == str; }
      template<mcsl::str_t str_t> constexpr bool operator!=(const str_t& str) { return _opStr != str; }

      constexpr bool operator==(const OpData& other) const { return _opID == other._opID; }
      constexpr bool operator< (const OpData& other) const { return _precedence <  other._precedence; }
      constexpr bool operator<=(const OpData& other) const { return _precedence <= other._precedence; }
      constexpr bool operator> (const OpData& other) const { return _precedence >  other._precedence; }
      constexpr bool operator>=(const OpData& other) const { return _precedence >= other._precedence; }

      void printf() const { return _opStr.printf(); }
};

struct clef::OpGroup {
   private:
      mcsl::arr_span<OpData> _ops;
      TokenType _type;
   public:
      constexpr OpGroup():_ops{},_type{} {}
      constexpr OpGroup(OpData* begin, const uint size):_ops{begin,size},_type{tokType(begin->toString())} {}

      constexpr uint size() const { return _ops.size(); }
      constexpr TokenType type() const { return _type; }
      constexpr OpData& operator[](const uint i) { return _ops[i]; }
      constexpr const OpData& operator[](const uint i) const { return _ops[i]; }

      constexpr auto toString() const { return _ops[0].toString(); }
      
      constexpr operator bool() const { return _ops.size(); }
};

// //!rework clef::OpData to list all possible operators for the operator string
// //!ordered map where longer matching substrings are less than shorter ones
// //!BST? unordered map of BSTs?

// //operator string + metadata
// struct clef::OpData {
//    mcsl::raw_buf_str<MAX_OP_LEN,byte> opStr;
//    byte precedence;
//    OpType opType; //unary/binary/special, left/right associative

//    constexpr OpData(): opStr(""),precedence(0),opType(static_cast<OpType>(0)) {}
//    constexpr OpData(const char str[MAX_OP_LEN + 1], const byte prec, const OpType type):
//       opStr(str),precedence(prec),opType(type) {}
   
//    constexpr byte size() const { return opStr.size(); }
// };
// //delimiter pair strings
// struct clef::DelimPair {
//    mcsl::raw_buf_str<MAX_OP_LEN, byte> open;
//    mcsl::raw_buf_str<MAX_OP_LEN, byte> close;
// };

#endif //OP_DATA_HPP