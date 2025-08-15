#ifndef LEXER_CPP
#define LEXER_CPP

#include "Lexer.hpp"
#include "Source.hpp"
#include "TokenData.hpp"
#include "OperatorData.hpp"
#include "KeywordData.hpp"

#include "char_type.hpp"
#include "str_to_num.hpp"
#include "unreachable.hpp"
#include "pretty-print.hpp"

//!lex the next token
clef::Token clef::Lexer::nextToken() {
RESTART:
   if (curr >= end) { [[unlikely]];
      return {};
   }
   mcsl::_::n val;
   KeywordID kw;

   //process token
   tokBegin = curr;
   switch (*curr) {
      //!NUMBERS
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
         val = mcsl::str_to_num(curr, end);
         curr += val.len;

         //skip trailing underscores (to allow hex literals to be marked as floats)
         while (curr < end && *curr == NUM_LIT_DELIM) {
            ++curr;
         }

         //numeric literal suffix
         if (curr < end && mcsl::is_letter(*curr)) {
            //uint, sint, or float
            char type = *curr++ | mcsl::CASE_BIT;
            switch (type) { //!TODO: use size specifiers
               case UINT_LIT_CHAR: kw = KeywordID::UINT; break;
               case SINT_LIT_CHAR: kw = KeywordID::SINT; break;
               case REAL_LIT_CHAR: kw = KeywordID::FLOAT; break;
               case CHAR_LIT_CHAR: kw = KeywordID::CHAR; break;

               default: logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("invalid literal type specifier `%c`"), type);
            }
            if (curr < end) {
               //architecture-dependant types
               if (mcsl::is_letter(*curr)) {
                  char size = *curr++ | mcsl::CASE_BIT;
                  kw = makeSized_c(kw, size);
                  if (kw == KeywordID::_NOT_A_KEYWORD) {
                     logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("invalid literal type specifier `%c%c`"), type, size);
                  }
               }
               //explicitly sized types
               else if (mcsl::is_digit(*curr)) {
                  mcsl::_::u typeSize;
                  typeSize = mcsl::str_to_uint(curr, end - curr, 10);
                  curr += typeSize.len;
                  kw = makeSized_n(kw, typeSize.val);
                  if (kw == KeywordID::_NOT_A_KEYWORD) {
                     logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("invalid literal type specifier `%c%u`"), type, typeSize.val);
                  }
               }
            }
         } else {
            kw = KeywordID::_NOT_A_KEYWORD;
         }
         if (curr < end && mcsl::is_letter(*curr)) {
            logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("identifier may not start with digit, and numeric literal must not be directly followed by identifier without separating whitespace"));
         }

         //convert to number and push token to stream
         switch (val.val.type) {
            case mcsl::NumType::null: UNREACHABLE;
            case mcsl::NumType::UINT:
               if (kw == KeywordID::_NOT_A_KEYWORD || isUint(kw)) {
                  return Token::makeUint(val.val.u, kw, {tokBegin, curr});
               } else if (isSint(kw)) {
                  return Token::makeSint(val.val.u, kw, {tokBegin, curr});
               } else if (isFloatingPoint(kw)) {
                  return Token::makeReal(val.val.u, kw, {tokBegin, curr});
               } else { [[unlikely]];
                  debug_assert(isText(kw));
                  TODO;
               }
            case mcsl::NumType::SINT: UNREACHABLE;
            case mcsl::NumType::REAL:
               if (kw == KeywordID::_NOT_A_KEYWORD || isFloatingPoint(kw)) {
                  return Token::makeReal(val.val.f, kw, {tokBegin, curr});
               } else { [[unlikely]];
                  logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("cannot narrow floating point literal to integer type `%s`"), toString(kw));
               }
         }
      UNREACHABLE;

      //ATTRIBUTES
      case '@':
         ++curr;
         fthru;

      //IDENTIFIERS
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'X': case 'o': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'Y': case 'Z':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'x': case 'O': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'y': case 'z':
      case '_': {
         bool isMacroInvoke = false;
         while (++curr < end && (mcsl::is_digit(*curr, 36) || *curr == '_')) {}
         if (curr < end && *curr == '!') {
            ++curr;
            isMacroInvoke = true;
         }
         mcsl::str_slice name{tokBegin,curr};
         KeywordID id = decodeKeyword(name);
         if (+id) {
            if (id == KeywordID::TRUE || id == KeywordID::FALSE) {
               return Token::makeBool(id == KeywordID::TRUE, {tokBegin, curr});
            }
            return Token::makeKeyword(id, {tokBegin, curr});
         } else {
            return Token::makeIden(name, isMacroInvoke, {tokBegin, curr});
         }
      }
      UNREACHABLE;


      //EOS
      case EOS:
         ++curr;
         return Token::makeEOS({tokBegin, curr});

      //ESCAPE CHAR
      case ESCAPE_CHAR:
         curr += 2;
         goto RESTART;

      //PREPROCESSOR
      case PREPROC_INIT:
         ++curr;
         return Token::makePreprocInit({tokBegin, curr});
      //WHITESPACE
      case '\n':
         ++lineIndex;
      case  ' ': case '\t': case '\v': case '\f': case '\r':
         ++curr;
         goto RESTART;
      
      //OPERATORS
      case '!': case '$': case '%': case '&': case '+': case ',': case '-': case '.': case '=': case '?': case '^': case '|': case '~': case '(': case ')': case '[': case ']': case '{': case '}': case '*': case '/': case '<': case '>': case ':': {
         OpData op = OPERATORS[mcsl::str_slice{curr, end}];
         debug_assert(op);
         curr += op.size();
         switch (op.tokType()) {
            case TokenType::OP: return Token::makeOp(op, {tokBegin, curr});
            case TokenType::BLOCK_DELIM:
               switch (op.op()) {
                  #define __DEF_IMPL(type, role) \
                     case Oplike::type##_##role: \
                        return Token::makeBlock( \
                           BlockType::type,      \
                           BlockDelimRole::role, \
                           op, {tokBegin, curr}  \
                        )
                  #define __DEF_BLOCK(type)  \
                     __DEF_IMPL(type, OPEN); \
                     __DEF_IMPL(type, CLOSE)

                  __DEF_BLOCK(CALL);
                  __DEF_BLOCK(INDEX);
                  __DEF_BLOCK(LIST);
                  __DEF_BLOCK(SPECIALIZER);
                  
                  #undef __DEF_BLOCK
                  #undef __DEF_IMPL

                  case Oplike::BLOCK_CMNT_OPEN: 
                     do {
                        if (curr >= end) { logError({{tokBegin, curr}}, ErrCode::BAD_CMNT, FMT("unclosed block comment")); }
                        OpData tmp = OPERATORS[mcsl::str_slice{curr,end}];
                        if (tmp == Oplike::BLOCK_CMNT_CLOSE) { curr += tmp.size(); break; }
                        ++curr;
                     } while (true);
                     goto RESTART;
                  case Oplike::BLOCK_CMNT_CLOSE: logError({{tokBegin, curr}}, ErrCode::BAD_CMNT, FMT("floating block comment closing delimiter"));

                  case Oplike::LINE_CMNT:
                     while (curr < end) {
                        if (*curr == '\n') { ++lineIndex; ++curr; break; }
                        if (*curr == '\\') { ++curr; }
                        ++curr;
                     }
                     goto RESTART;

                  default: UNREACHABLE;
               }
               UNREACHABLE;

            default: UNREACHABLE;
         }

      } break;

      //CHARS
      case CHAR_DELIM:
         return lexChar();

      //STRINGS
      case STR_DELIM:
         return lexStr();

      //INTERPOLATED STRINGS
      case INTERP_STR_DELIM:
         return lexInterpStr();
      
      //UNPRINTABLE CHAR (illegal)
      default:
         debug_assert(*curr < 32 || *curr > 126);
         logError({{tokBegin, curr}}, ErrCode::UNREC_SRC_CHAR, FMT("invalid character (%u)"), *curr);
   }

   UNREACHABLE;
}

//!lex a character literal
//!NOTE: CURRENTLY ONLY SUPPORTS ASCII CHARACTER LITERALS
char clef::Lexer::parseChar() {
   char ch;
   if (*curr >= 32 && *curr <= 126) {
      if (*curr == ESCAPE_CHAR) {
         if (++curr >= end) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("incomplete character literal")); }
         switch (*curr) {
            #pragma region numescseq
            case 'b': case 'B': { //1-8 binary digits
               char* numBegin = ++curr;
               char* numEnd = numBegin + MAX_BIN_CHAR_LIT_DIGITS;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 2)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (curr == numBegin) { ch = '\b'; }
               else if (numEnd != curr) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("bad binary numeric escape sequence")); }
               else { ch = (char)mcsl::str_to_uint(numBegin, numEnd, 2); }
               break;
            }
            case 'o': case 'O': { //1-3 octal digits
               //extract digit sequence
               char* numBegin = ++curr;
               char* numEnd = numBegin + MAX_OCT_CHAR_LIT_DIGITS;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 8)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("bad octal numeric escape sequence")); }
               //parse digit sequence
               do { //!NOTE: kinda unsafe, but should always work if implemented correctly
                  debug_assert(numEnd > numBegin);
                  ushort tmp = (ushort)mcsl::str_to_uint(numBegin, numEnd, 8);
                  if (tmp == (char)tmp) { ch = (char)tmp; break; }
                  --numEnd;
               } while (true);
               curr = numEnd;
               break;
            }
            case 'd': case 'D': { //1-3 decimal digits
               //extract digit sequence
               char* numBegin = ++curr;
               char* numEnd = numBegin + MAX_DEC_CHAR_LIT_DIGITS;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 10)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("bad decimal numeric escape sequence")); }
               //parse digit sequence
               do { //!NOTE: kinda unsafe, but should always work if implemented correctly
                  debug_assert(numEnd > numBegin);
                  ushort tmp = (ushort)mcsl::str_to_uint(numBegin, numEnd, 10);
                  if (tmp == (char)tmp) { ch = (char)tmp; break; }
                  --numEnd;
               } while (true);
               curr = numEnd;
               break;
            }
            case 'x': case 'X': { //1-2 hex digits
               char* numBegin = ++curr;
               char* numEnd = numBegin + MAX_HEX_CHAR_LIT_DIGITS;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 16)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("bad hexidecimal numeric escape sequence")); }
               ch = (char)mcsl::str_to_uint(numBegin, numEnd, 16);
               break;
            }
            #pragma endregion numescseq

            // case 'a': ch = '\a'; ++curr; break;
            // case 'b': ch = '\b'; ++curr; break;
            case 'e': ch = '\033'; ++curr; break;
            case 'f': ch = '\f'; ++curr; break;
            case 'n': ch = '\n'; ++curr; break;
            case 'r': ch = '\r'; ++curr; break;
            case 't': ch = '\t'; ++curr; break;
            case 'v': ch = '\v'; ++curr; break;

            default: if (*curr < 32 || *curr > 126) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("unprintable escaped character literal (%u)"), *curr); }
               //!TODO: warning for unrecognized escape sequence
               fthru;
            case '\'': fthru;
            case '\"': fthru;
            case '\\':
               ch = *curr;
               ++curr;
               break;
         }
      } else {
         ch = *curr;
         ++curr;
      }
   } else { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("unprintable character literal (%u)"), *curr); }

   return ch;
}

clef::Token clef::Lexer::lexChar() {
   ++curr; //skip opening quote
   const char tmp = parseChar();
   if (curr >= end || *curr != CHAR_DELIM) { logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("character literal may only contain a single character/escape sequence")); }
   ++curr; //skip closing quote
   return Token::makeChar(tmp, {tokBegin, curr});
}
clef::Token clef::Lexer::lexStr() {
   while (++curr < end && *curr != STR_DELIM) {
      if (*curr == ESCAPE_CHAR) {
         ++curr;
      }
   }
   if (curr >= end || *curr != STR_DELIM) {
      logError({{tokBegin, curr}}, ErrCode::BAD_LIT, FMT("unclosed string literal"));
   }
   ++curr;
   return Token::makeStr(mcsl::str_slice{tokBegin+1,curr-1}, {tokBegin, curr});
}
clef::Token clef::Lexer::lexInterpStr() {
   TODO;
}
clef::Token clef::Lexer::lexExpr() {
   TODO;
}

mcsl::str_slice clef::Lexer::currLine() {
   mcsl::str_slice line = src.line(lineIndex);
   debug_assert(curr >= line.begin());
   debug_assert(curr < line.end());
   return line;
}

#endif //LEXER_CPP