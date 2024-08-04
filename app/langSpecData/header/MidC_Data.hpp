#pragma once
#ifndef DATA_HPP
#define DATA_HPP

#include "CLEF.hpp"
#include "Hash.hpp"

#include "raw_str_span.hpp"
#include "raw_str.hpp"
#include "static_arr.hpp"
#include "pair.hpp"

#include <cassert>
#include <cstring>

namespace clef {
   extern const mcs::static_arr<TokenType,256> tokTypeArr;

   extern const mcs::static_arr<Operator,73> OPERATORS;
   extern const mcs::static_arr<DelimPair,7> BLOCK_DELIMS;
   // extern const mcs::static_arr<mcs::string> KEYWORDS;
   
   
   //block delims
   DelimPairType blockDelimType(const mcs::raw_str_span& str);

   //operators
   uint maxOpLen(const mcs::raw_str_span& str);
   const Operator* getOpData(const mcs::raw_str_span& str);

   //!function to determine if a string is a Middle-C keyword
   inline bool isKeyword(const mcs::raw_str_span& str) {return Hash::isKeyword(str.begin(), str.size());}
}

//operator string + metadata
struct clef::Operator {
   mcs::raw_str<MAX_OP_LEN + 1> opStr;
   byte size;
   byte precedence;
   OpType opType; //unary/binary/special, left/right associative

   constexpr Operator() {
      std::memset(this,0,sizeof(Operator));
   }
   constexpr Operator(const char str[MAX_OP_LEN + 1], const byte prec, const OpType type) {
      size = std::strlen(str);
      assert(size < MAX_OP_LEN + 1);                  //bounds-checking
      std::memcpy(opStr.begin(),str,size*sizeof(char));       //copy string
      std::memset(opStr.begin()+size,0,sizeof(opStr)-size);   //null-initialize the rest of the string
      precedence = prec;
      opType = type;
   }
};
//delimiter pair strings
struct clef::DelimPair {
   mcs::raw_str<MAX_OP_LEN + 1> open;
   mcs::raw_str<MAX_OP_LEN + 1> close;
};

#endif //DATA_HPP