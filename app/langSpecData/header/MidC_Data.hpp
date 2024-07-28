#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"
#include "Hash.hpp"

#include "string.hpp"
#include "array.hpp"
#include "pair.hpp"

#include <cassert>
#include <cstring>

namespace clef {
   extern TokenType tokTypeArr[256];

   extern const mcs::array<Operator> OPERATORS;
   extern const mcs::array<DelimPair> BLOCK_DELIMS;
   // extern const char KEYWORDS[][MAX_KEYWORD_LEN + 1];
   // extern const mcs::array<mcs::string> KEYWORDS;
   
   
   //block delims
   DelimPairType blockDelimType(const char* str, const uint length);

   //operators
   uint maxOpLen(const char* str, const uint len);
   const Operator* getOpData(const char* str, const uint len);

   //!function to determine if a string is a Middle-C keyword
   inline bool isKeyword(const char* ch, const uint length) {return Hash::isKeyword(ch, length);}
}

//operator string + metadata
struct clef::Operator {
   char opStr[MAX_OP_LEN + 1];
   byte size;
   byte precedence;
   OpType opType; //unary/binary/special, left/right associative

   Operator() {
      std::memset(this,0,sizeof(Operator));
   }
   Operator(const char str[MAX_OP_LEN + 1], const byte prec, const OpType type) {
      size = std::strlen(str);
      assert(size < MAX_OP_LEN + 1);                  //bounds-checking
      std::memcpy(opStr,str,size*sizeof(char));       //copy string
      std::memset(opStr+size,0,sizeof(opStr)-size);   //null-initialize the rest of the string
      precedence = prec;
      opType = type;
   }
};
//delimiter pair strings
struct clef::DelimPair {
   char open[MAX_DELIM_LEN + 1];
   char close[MAX_DELIM_LEN + 1];

   // DelimPair() {
   //    std::memset(this,0,sizeof(DelimPair));
   // }
   // DelimPair(const char o[MAX_DELIM_LEN + 1], const char c[MAX_DELIM_LEN + 1]) {
   //    std::memcpy(open,o,sizeof(open));
   //    std::memcpy(close,c,sizeof(close));
   // }
};

#endif //DATA_HPP