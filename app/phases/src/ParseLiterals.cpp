#ifndef PARSE_LITERALS_CPP
#define PARSE_LITERALS_CPP

#include "Parser.hpp"

#include "str_to_num.hpp"

//!parse the contents of an ASCII character literal block
char clef::Parser::parseCharLitASCII(const char* begin, const char* end) {
   //check that the string is valid and non-empty
   if (end <= begin || !begin) { //NOTE: don't need to check that end is non-null because it can't be greater than begin if it is
      return -1;
   }

   const uint strlen = end - begin;
   if (begin[0] == '\\') {
      if (strlen < 2) { return -1; }
      switch (begin[1]) {
         //HEX
         case 'x':
            if (strlen > 4) { return -1; }
            return mcsl::is_uint(begin+2, end, 16) ? mcsl::str_to_uint(begin+2, end, 16) : -1;
         //OCTAL
         case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
            if (strlen > 4) { return -1; }
            return mcsl::is_uint(begin+1, end, 8) ? mcsl::str_to_uint(begin+1, end, 8) : -1;
         
         //ESCAPED CHARACTER
         case  'a': return (strlen == 2) ? '\a' : -1;
         case  'b': return (strlen == 2) ? '\b' : -1;
         // case  'e': return (strlen == 2) ? '\e' : -1;
         case  'f': return (strlen == 2) ? '\f' : -1;
         case  'n': return (strlen == 2) ? '\n' : -1;
         case  'r': return (strlen == 2) ? '\r' : -1;
         case  't': return (strlen == 2) ? '\t' : -1;
         case  'v': return (strlen == 2) ? '\v' : -1;
         case '\\': return (strlen == 2) ? '\\' : -1;
         case '\'': return (strlen == 2) ? '\'' : -1;
         case '\"': return (strlen == 2) ? '\"' : -1;
         
         default: return -1;
      }

   } else {
      return (strlen == 1) ? begin[0] : -1;
   }
}

//!parse the contents of an ASCII string literal block
mcsl::string clef::Parser::parseStrLitASCII(const char* begin, const char* end) {
   mcsl::string str{};
   for (const char* it = begin; it < end;) {
      if (*it != '\\') {   //!STANDARD CHAR
         debug_assert(*it);
         str.push_back(*it);
         ++it;
      }
      else {               //!ESCAPE CHAR
         debug_assert((end - it) < 2);

         switch (it[1]) {
            //HEX
            case 'x':
               it += 2; // skip the \x
               if ((it >= end) || !mcsl::is_digit(*it, 16)) { logError<true>(ErrCode::BAD_LITERAL, "bad escaped hex value in string literal"); }
               str.push_back(mcsl::str_to_uint(it, end, 16));

               ++it;
               if (it < end && mcsl::is_digit(*it, 16)) { ++it; }
               break;
            //OCTAL
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
               ++it; // skip the backslash
               str.push_back(mcsl::str_to_uint(it, end, 8));
               
               ++it;
               if (it < end && mcsl::is_digit(*it, 8)) { ++it; } else { break; }
               if (it < end && mcsl::is_digit(*it, 8)) { ++it; }
               break;
            
            //ESCAPED CHARACTER
            case  'a': str.push_back('\a'); it += 2; break;
            case  'b': str.push_back('\b'); it += 2; break;
            // case  'e': str.push_back('\e'); it += 2; break;
            case  'f': str.push_back('\f'); it += 2; break;
            case  'n': str.push_back('\n'); it += 2; break;
            case  'r': str.push_back('\r'); it += 2; break;
            case  't': str.push_back('\t'); it += 2; break;
            case  'v': str.push_back('\v'); it += 2; break;
            case '\\': str.push_back('\\'); it += 2; break;
            case '\'': str.push_back('\''); it += 2; break;
            case '\"': str.push_back('\"'); it += 2; break;
            
            default: logError<true>(ErrCode::BAD_LITERAL, "bad escape code in string literal");
         }
      }
   }

   return str;
}

//!parse a numeric literal
bool clef::Parser::parseNumLit(SyntaxTree& tree, const Token*& tokIt, const Token* endtok) {
   LitType type = LitType::UINT;
   uint radix = 0;

   //check if it has a radix specifier
   if (tokIt->size() >= 2 && (*tokIt)[0] == '0') {
      switch ((*tokIt)[1]) {
         case 'b': case 'B': radix = 2; break;
         case 'o': case 'O': radix = 8; break;
         case 'd': case 'D': radix = 10; break;
         case 'x': case 'X': radix = 16; break;

         default: break;
      }
   }
   
   const Token* it = tokIt + 1;
   if (it >= endtok) { goto STR_TO_NUM; }
   //radix point
   if (it->type() == TokenType::OP && OPERATORS[*it] == RADIX_POINT) {
      ++it;
      if (it >= endtok) { goto STR_TO_NUM; }
      if (it->type() == TokenType::NUM || it->type() == TokenType::DGIT || it->type() == TokenType::XDGT) {
         type = LitType::FLOAT;
         ++it;
         if (it >= endtok) { goto STR_TO_NUM; }
      } else {
         goto BAD_NUM;
      }
   }
   //radix separator
   if (it->type() == TokenType::CHAR && (*it) == RADIX_SEPARATOR) { //p
      ++it;
      if (it >= endtok) { goto STR_TO_NUM; }
      //sign
      if (it->type() == TokenType::OP && [&](){ const auto tmp = OPERATORS[*it]; return tmp == ADDITION || tmp == SUBTRACTION; }()) {
         ++it;
         if (it >= endtok) { goto STR_TO_NUM; }
      }
      //digits
      if (it->type() == TokenType::NUM || it->type() == TokenType::DGIT || it->type() == TokenType::XDGT) {
         type = LitType::FLOAT;
         ++it;
         if (it >= endtok) { goto STR_TO_NUM; }
      }
   } else if (radix < 16) {
      if (it->back() == DECIMAL_RADIX_SEPARATOR) { //e
         ++it;
         if (it >= endtok) { goto STR_TO_NUM; }
         //sign
         if (it->type() == TokenType::OP && [&](){ const auto tmp = OPERATORS[*it]; return tmp == ADDITION || tmp == SUBTRACTION; }()) {
            ++it;
            if (it >= endtok) { goto STR_TO_NUM; }
         }
         //digits
         if (it->type() == TokenType::NUM || it->type() == TokenType::DGIT || it->type() == TokenType::XDGT) {
            type = LitType::FLOAT;
            ++it;
            if (it >= endtok) { goto STR_TO_NUM; }
         }
      }
      else {
         for (uint i = it->size() - 1; mcsl::carry::SUB(i, 1); ) { //search for radix separator (back-to-front)
            if ((*tokIt)[i] == DECIMAL_RADIX_SEPARATOR) {
               type = LitType::FLOAT;
               goto STR_TO_NUM;
            }
         }
      }
   }

   STR_TO_NUM: {
      --it;

      const char* begin = tokIt->begin();
      if (radix) {
         begin += 2;
      } else { radix = 10; }
      const uint strlen = begin - it->end();
      
      switch (type) {
         case LitType::UINT:
            const ulong val = mcsl::str_to_uint(begin, strlen, radix);
            //!NOTE: UNFINISHED

            tokIt = it;
            return true;

         case LitType::FLOAT:
            const double val = mcsl::str_to_real(begin, strlen, radix);
            //!NOTE: UNFINISHED

            tokIt = it;
            return true;

         default: //!NOTE: SHOULD BE IMPOSSIBLE
            //!NOTE: UNFINISHED
            throw "make a proper error code for this";
      }
   }

   BAD_NUM:
   //!NOTE: UNFINISHED

}

#endif //PARSE_LITERALS_CPP