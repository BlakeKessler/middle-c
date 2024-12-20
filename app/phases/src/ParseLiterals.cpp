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
   const Token* it = tokIt + 1;
   LitType type = LitType::UINT;
   if (it >= endtok || it->type() != TokenType::OP || tokIt->end() != it->begin()|| OPERATORS[*it] != RADIX_POINT) {
      goto STR_TO_NUM;
   }
   type = LitType::FLOAT;
   ++it;
   if (it >= endtok || (it->type() != TokenType::DGIT && it->type() != TokenType::NUM && it->type() != TokenType::XDGT) || (it-1)->end() != it->begin()) {
      type = LitType::UINT;
      goto STR_TO_NUM;
   }

   //!NOTE: UNFINISHED

   STR_TO_NUM:

}

#endif //PARSE_LITERALS_CPP