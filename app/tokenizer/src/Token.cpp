#ifndef TOKEN_CPP
#define TOKEN_CPP

#include "Token.hpp"
#include "MidC_Data.hpp"
#include <ostream>
#include <cstring>
#include <cassert>

//constructors
clef::Token::Token():
mcsl::raw_str_span(),_lineNum(0) {

}
clef::Token::Token(char* front, char* back, const uint line):
mcsl::raw_str_span(front, back - front),_lineNum(line) {

}
clef::Token::Token(char* front, const uint length, const uint line):
mcsl::raw_str_span(front, length),_lineNum(line) {

}
clef::Token::Token(Token&& other):
mcsl::raw_str_span(std::move(other)),_lineNum(other._lineNum) {}

clef::Token clef::Token::split(const uint i) {
   assert(i <= size());
   const uint origSize = size();
   set_size(i);
   return Token(end(), origSize-i, _lineNum);
}

//!process typeNumArr values to work with the tokenizer while loop
clef::TokenType clef::Token::typeNum(const char ch, const bool isstart) {
   const TokenType temp = tokTypeArr[+ch];
   return (temp & ~(TokenType::IDEN * (isstart && !+(temp & TokenType::STRT))));
}
//!calculate the type number for a token
clef::TokenType clef::Token::typeNum() const {
   const char* current = begin();
   const char* end = current + size();

   TokenType num = Token::typeNum(*current, true);
   while (++current < end) {
      num = num & Token::typeNum(*current, false);
   }
   return num;
}
//!find the type of the longest token in a string that starts at the first char
//!tokEnd will be set to the end of the found token string
uint clef::Token::findTokEnd(const char* const tokStr, const uint len) {
   if (!tokStr || !len) {
      return len;
   }
   //iterate until no longer creates a valid token
   uint i = 0;
   TokenType typeNum = Token::typeNum(tokStr[i], true);
   while (+typeNum && (++i < len)) {
      typeNum = typeNum & Token::typeNum(tokStr[i], false);
   }
   //skip chars that can't be the start of a token if tokStr starts with them 
   if (!i) {
      while ((++i < len) && !+Token::typeNum(tokStr[i],true)) {}
   }
   //return pointer to end char if it doesn't create a null token
   return i ? i : len;
}

//!get delimiter pair data block
const clef::DelimPair* clef::Token::getDelimPairData() const {
   if (!size() || size() > MAX_DELIM_LEN) {
      return nullptr;
   }

   //search array
   for (uint i = 1; i < BLOCK_DELIMS.size(); ++i) {
      if (self == BLOCK_DELIMS[i].open || self == BLOCK_DELIMS[i].close) {
         return &BLOCK_DELIMS[i];
      }
   }
   return nullptr;
}
//!get plaintext delimiter pair data block
const clef::DelimPair* clef::Token::getPtxtData() const {
   if (!size() || size() > MAX_DELIM_LEN) {
      return nullptr;
   }

   //search array
   for (uint i = 1; i < PTXT_DELIMS.size(); ++i) {
      if (self == PTXT_DELIMS[i].open || self == PTXT_DELIMS[i].close) {
         return &PTXT_DELIMS[i];
      }
   }
   return nullptr;
}

//!throw error for token
void clef::Token::throwError(const ErrCode code) const {
   clef::throwError(code, "token: \033[4m%.*s\033[24m", size(), begin());
}


//!NOTE: re-write to not be a huge wall of if statements
clef::NodeType clef::Token::nodeType() const {
   const TokenType type = typeNum();
   if (+(type & TokenType::EOS)) {
      return NodeType::EOS;
   }
   if (+(type & TokenType::CHAR)) {
      return isKeyword() ? NodeType::KEYWORD : NodeType::IDEN;
   }
   if (+(type & TokenType::NUM)) {
      return NodeType::LITERAL;
   }
   if (+(type & TokenType::OP)) {
      return 
         +(type & (TokenType::BLOC | TokenType::PTXT)) ?
            NodeType::OP_OR_DELIM :
         self == mcsl::raw_str("\\") ? NodeType::ESCAPE : NodeType::OPERATOR;
   }
   if (+(type & TokenType::BLOC)) {
      return NodeType::DELIM_GEN;
   }
   if (+(type & TokenType::PTXT)) {
      return NodeType::PTXT_DELIM;
   }
   return NodeType::NONE;
}

#endif //TOKEN_CPP