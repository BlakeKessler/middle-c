#ifndef TOKEN_CPP
#define TOKEN_CPP

#include "Token.hpp"
#include "MidC_Data.hpp"
#include <ostream>
#include <cstring>
#include <cassert>

//constructors
clef::Token::Token():
StrView(),_lineNum(0) {

}
clef::Token::Token(char* front, char* back, const uint line):
StrView(front, back),_lineNum(line) {

}
clef::Token::Token(char* front, const uint length, const uint line):
StrView(front, length),_lineNum(line) {

}

clef::Token clef::Token::split(const uint i) {
   uint len = size();
   assert(len >= i);
   _size = i;
   return Token(end(), len-i, _lineNum);
}

//!check if a token and a token string are identical
bool clef::Token::operator==(const std::string& other) const {
   if (_size != other.size()) {
      return false;
   }
   for (uint i = 0; i < _size; ++i) {
      if (_str[i] != other[i]) {
         return false;
      }
   }
   return true;
}

//!process typeNumArr values to work with the tokenizer while loop
byte clef::Token::typeNum(const char ch, const bool isstart) {
   const byte temp = tokTypeArr[+ch];
   return temp & ~(IDEN * (isstart && !(temp & STRT)));
}
//!calculate the type number for a token
byte clef::Token::typeNum() const {
   char* current = _str;
   char* end = current + _size;

   byte num = Token::typeNum(*current, true);
   while (++current < end) {
      num &= Token::typeNum(*current, false);
   }
   return num;
}
//!find the type of the longest token in a string that starts at the first char
//!tokEnd will be set to the end of the found token string
uint clef::Token::findTokEnd(char* const tokStr, const uint len) {
   if (!tokStr || !len) {
      return len;
   }
   //iterate until no longer creates a valid token
   uint i = 0;
   byte typeNum = Token::typeNum(tokStr[i], true);
   while (typeNum && (++i < len)) {
      typeNum &= Token::typeNum(tokStr[i], false);
   }
   //return pointer to end char if it doesn't create a null token
   return i ? i : len;
}

//!compare token string and raw string for exact equality
bool clef::Token::tokcmp(const Token* tok, const char* str, const uint strlen) {
   return (tok && str) && (tok->size() == strlen) && !(strcmp(tok->data(), str, strlen));
}

//!print token to stream
std::ostream& clef::operator<<(std::ostream& stream, const clef::Token& token) {
   stream.write(token.data(), token.size());
   return stream;
}
//!print token to stream
std::ostream& clef::operator<<(std::ostream& stream, const clef::Token* token) {
   return token ? (stream << *token) : stream;
}

//!throw error for token
void clef::Token::throwError(const ErrCode code) const {
   clef::throwError(code, "token: \033[4m%.*s\033[24m", _size, _str);
}



clef::NodeType clef::Token::nodeType() const {
   const byte type = typeNum();
   if (type & EOS) {
      return NodeType::EOS;
   }
   if (type & CHAR) {
      return isKeyword() ? NodeType::KEYWORD : NodeType::IDEN;
   }
   if (type & NUM) {
      return NodeType::LITERAL;
   }
   if (type & OP) {
      return (type & (BLOC | PTXT)) ?
         NodeType::OP_OR_DELIM : NodeType::OPERATOR;
   }
   if (type & (BLOC | PTXT)) {
      return NodeType::DELIM_GEN;
   }
   return NodeType::NONE;
}

#endif //TOKEN_CPP