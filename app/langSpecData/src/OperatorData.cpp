#ifndef OPERATORS_CPP
#define OPERATORS_CPP

#include "MidC_Data.hpp"

//!find the length of the longest Middle C operator contained by the string
uint clef::maxOpLen(const mcs::raw_str_span& str) {
   uint maxlen = 0;
   for (uint i = 0; i < OPERATORS.size(); ++i) {
      //check length
      if (str.size() < OPERATORS[i].size) {
         continue;
      }
      //find first differing char
      for (uint j = 0; j < OPERATORS[i].size; ++j) {
         if (str[j] == OPERATORS[i].opStr[j]) {
            maxlen = j >= maxlen ? j + 1 : maxlen;
         }
         else {
            break;
         }
      }
   }

   //triangle-bracket-enclosed operators
   return (((maxlen < 3 && str.size() >= 3)
         && (str[0] == '<' || str[0] == '>') && (str[2] == '<' || str[2] == '>'))
         && +(tokTypeArr[+str[1]] & TokenType::OP))
      ? 3 : maxlen;
}

//!find the length of the longest Middle C operator contained by the string
//!NOTE: replace with hash map
const clef::Operator* clef::getOpData(const mcs::raw_str_span& str, bool banBinary) {
   uint maxlen = 0;
   const Operator* op = nullptr;
   for (uint i = 0; i < OPERATORS.size(); ++i) {
      //handle unary vs. binary
      if (banBinary && !+(OPERATORS[i].opType & OpType::BIN)) {
         continue;
      }
      //check length
      if (OPERATORS[i].size > str.size()) {
         continue;
      }
      //find first differing char
      for (uint j = 0; j < OPERATORS[i].size; ++j) {
         if (str[j] == OPERATORS[i].opStr[j]) {
            if (j >= maxlen) {
               maxlen = j + 1;
               op = &OPERATORS[i];
            }
         }
         else {
            break;
         }
      }
   }

   return op;
}

#endif //OPERATORS_CPP