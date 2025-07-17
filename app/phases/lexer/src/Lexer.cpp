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
            switch (type) {
               case UINT_LIT_CHAR: kw = KeywordID::UINT; break;
               case SINT_LIT_CHAR: kw = KeywordID::SINT; break;
               case REAL_LIT_CHAR: kw = KeywordID::FLOAT; break;
               case CHAR_LIT_CHAR: kw = KeywordID::CHAR; break;

               default: logError(ErrCode::BAD_LITERAL, "invalid literal type specifier `%c`", type);
            }
            if (curr < end) {
               //architecture-dependant types
               if (mcsl::is_letter(*curr)) {
                  char size = *curr++ | mcsl::CASE_BIT;
                  kw = makeSized_c(kw, size);
                  if (kw == KeywordID::_NOT_A_KEYWORD) {
                     logError(ErrCode::BAD_LITERAL, "invalid literal type specifier `%c%c`", type, size);
                  }
               }
               //explicitly sized types
               else if (mcsl::is_digit(*curr)) {
                  mcsl::_::u typeSize;
                  typeSize = mcsl::str_to_uint(curr, end - curr, 10);
                  curr += typeSize.len;
                  kw = makeSized_n(kw, typeSize.val);
                  if (kw == KeywordID::_NOT_A_KEYWORD) {
                     logError(ErrCode::BAD_LITERAL, "invalid literal type specifier `%c%u`", type, typeSize.val);
                  }
               }
            }
         } else {
            kw = KeywordID::_NOT_A_KEYWORD;
         }
         if (curr < end && mcsl::is_letter(*curr)) {
            logError(ErrCode::BAD_LITERAL, "identifier may not start with digit, and numeric literal must not be directly followed by identifier without separating whitespace");
         }

         //!TODO: use kw

         //convert to number and push token to stream
         switch (val.val.type) {
            case mcsl::NumType::null: UNREACHABLE;
            case mcsl::NumType::UINT:
               if (kw == KeywordID::_NOT_A_KEYWORD || isUint(kw)) {
                  return {val.val.u, kw};
               } else if (isSint(kw)) {
                  return {(slong)val.val.u, kw};
               } else if (isFloatingPoint(kw)) {
                  return {(flong)val.val.u, kw};
               } else { [[unlikely]];
                  debug_assert(isText(kw));
                  TODO;
               }
            case mcsl::NumType::SINT: UNREACHABLE;
            case mcsl::NumType::REAL:
               if (kw == KeywordID::_NOT_A_KEYWORD || isFloatingPoint(kw)) {
                  return {val.val.f, kw};
               } else { [[unlikely]];
                  logError(ErrCode::BAD_LITERAL, "cannot narrow floating point literal to integer type `%s`", toString(kw));
               }
         }
      UNREACHABLE;

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
            return {id};
         } else {
            return {name, isMacroInvoke};
         }
      }
      UNREACHABLE;


      //EOS
      case EOS:
         ++curr;
         return {TokenType::EOS};

      //ESCAPE CHAR
      case ESCAPE_CHAR:
         curr += 2;
         goto RESTART;

      //PREPROCESSOR
      case PREPROC_INIT:
         ++curr;
         return {TokenType::PREPROC_INIT};
      //WHITESPACE
      case '\n':
         ++lineIndex;
      case  ' ': case '\t': case '\v': case '\f': case '\r':
         ++curr;
         goto RESTART;
      
      //OPERATORS
      case '!': case '$': case '%': case '&': case '+': case ',': case '-': case '.': case '=': case '?': case '@': case '^': case '|': case '~': case '(': case ')': case '[': case ']': case '{': case '}': case '*': case '/': case '<': case '>': case ':': {
         OpData op = OPERATORS[mcsl::str_slice{curr, end}];
         debug_assert(op);
         curr += op.size();
         switch (op.tokType()) {
            case TokenType::OP: return {op};
            case TokenType::BLOCK_DELIM:
               switch (op.opID()) {
                  case OpID::CALL_INVOKE: UNREACHABLE;
                  case OpID::CALL_OPEN  : return {BlockType::CALL, BlockDelimRole::OPEN};
                  case OpID::CALL_CLOSE : return {BlockType::CALL, BlockDelimRole::CLOSE};

                  case OpID::SUBSCRIPT_INVOKE: UNREACHABLE;
                  case OpID::SUBSCRIPT_OPEN  : return {BlockType::SUBSCRIPT, BlockDelimRole::OPEN};
                  case OpID::SUBSCRIPT_CLOSE : return {BlockType::SUBSCRIPT, BlockDelimRole::CLOSE};
                  
                  case OpID::LIST_INVOKE: UNREACHABLE;
                  case OpID::LIST_OPEN  : return {BlockType::INIT_LIST, BlockDelimRole::OPEN};
                  case OpID::LIST_CLOSE : return {BlockType::INIT_LIST, BlockDelimRole::CLOSE};

                  case OpID::SPECIALIZER_INVOKE: UNREACHABLE;
                  case OpID::SPECIALIZER_OPEN  : return {BlockType::SPECIALIZER, BlockDelimRole::OPEN};
                  case OpID::SPECIALIZER_CLOSE : return {BlockType::SPECIALIZER, BlockDelimRole::CLOSE};


                  case OpID::BLOCK_CMNT: UNREACHABLE;
                  case OpID::BLOCK_CMNT_OPEN: 
                     do {
                        if (curr >= end) { logError(ErrCode::BAD_CMNT, "unclosed block comment"); }
                        OpData tmp = OPERATORS[mcsl::str_slice{curr,end}];
                        if (tmp == OpID::BLOCK_CMNT_CLOSE) { curr += tmp.size(); break; }
                        ++curr;
                     } while (true);
                     goto RESTART;
                  case OpID::BLOCK_CMNT_CLOSE: logError(ErrCode::BAD_CMNT, "floating block comment closing delimiter");

                  case OpID::LINE_CMNT:
                     while (curr < end) {
                        if (*curr == '\n') { ++lineIndex; ++curr; break; }
                        if (*curr == '\\') { ++curr; }
                        ++curr;
                     }
                     goto RESTART;

                  default: UNREACHABLE;
               }
               UNREACHABLE;

            case TokenType::PTXT_SEG: UNREACHABLE;

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
         logError(ErrCode::LEXER_UNSPEC, "invalid character (%u)", *curr);
   }

   UNREACHABLE;
}

//!lex a character literal
//!NOTE: CURRENTLY ONLY SUPPORTS ASCII CHARACTER LITERALS
char clef::Lexer::parseChar() {
   char ch;
   if (*curr >= 32 && *curr <= 126) {
      if (*curr == ESCAPE_CHAR) {
         if (++curr >= end) { logError(ErrCode::BAD_LITERAL, "incomplete character literal"); }
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
               else if (numEnd != curr) { logError(ErrCode::BAD_LITERAL, "bad binary numeric escape sequence"); }
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
               if (numEnd != curr) { logError(ErrCode::BAD_LITERAL, "bad octal numeric escape sequence"); }
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
               if (numEnd != curr) { logError(ErrCode::BAD_LITERAL, "bad decimal numeric escape sequence"); }
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
               if (numEnd != curr) { logError(ErrCode::BAD_LITERAL, "bad hexidecimal numeric escape sequence"); }
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

            default: if (*curr < 32 || *curr > 126) { logError(ErrCode::BAD_LITERAL, "unprintable escaped character literal (%u)", *curr); }
               //!TODO: warning for unrecognized escape sequence
               [[fallthrough]];
            case '\'': [[fallthrough]];
            case '\"': [[fallthrough]];
            case '\\':
               ch = *curr;
               ++curr;
               break;
         }
      } else {
         ch = *curr;
         ++curr;
      }
   } else { logError(ErrCode::BAD_LITERAL, "unprintable character literal (%u)", *curr); }

   return ch;
}

clef::Token clef::Lexer::lexChar() {
   ++curr; //skip opening quote
   const char tmp = parseChar();
   if (curr >= end || *curr != CHAR_DELIM) { logError(ErrCode::BAD_LITERAL, "character literal may only contain a single character/escape sequence"); }
   ++curr; //skip closing quote
   return {tmp};
}
clef::Token clef::Lexer::lexStr() {
   while (++curr < end && *curr != STR_DELIM) {
      if (*curr == ESCAPE_CHAR) {
         ++curr;
      }
   }
   if (curr >= end || *curr != STR_DELIM) {
      logError(ErrCode::BAD_LITERAL, "unclosed string literal");
   }
   ++curr;
   return {mcsl::str_slice{tokBegin+1,curr-1}, PtxtType::UNPROCESSED_STR};
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