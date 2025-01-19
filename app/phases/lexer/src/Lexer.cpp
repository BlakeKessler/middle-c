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

//!creates a SourceTokens from results of file reader
clef::SourceTokens clef::Lexer::LexSource(Source&& src) {
   SourceTokens tokens(std::move(src));
   auto& toks = tokens.tokens();

   char* const begin = tokens.source().buf().begin();
   char* const end = tokens.source().buf().end();
   char* curr = begin;
   char* tokBegin;

   uint radix;
   bool isReal;
   bool lexingPreprocStmt = false;

   while (curr < end) {
      //reset data
      tokBegin = curr;

      //process token
      switch (*curr) {
         //!NUMBERS
         case '0':
            isReal = false;
            //check for radix specifier
            if (++curr >= end) { radix = 10; goto PUSH_NUM_TOK; }
            switch (*curr) {
               default:
                  radix = 10;
                  if (!mcsl::is_digit(*curr, radix)) { goto PUSH_NUM_TOK; }
                  goto PROCESS_NUM;

               case 'b': case 'B': radix = 2;  break;
               case 'o': case 'O': radix = 8;  break;
               case 'd': case 'D': radix = 10; break;
               case 'x': case 'X': radix = 16; break;
            }
            if (++curr >= end || !mcsl::is_digit(*curr, radix)) {
               throwError(ErrCode::BAD_LITERAL, "radix specifier must be followed by digits");
            }
            goto PROCESS_NUM;

         //!NUMBERS
         case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            //setup
            radix = 10;
            isReal = false;
PROCESS_NUM:
            //integer part
            do {
               if (++curr >= end) { goto PUSH_NUM_TOK; }
            } while (mcsl::is_digit(*curr, radix));

            //radix point
            if (*curr == RADIX_POINT) {
               isReal = true;
               //check first digit
               if (++curr >= end || !mcsl::is_digit(*curr, radix)) {
                  throwError(ErrCode::BAD_LITERAL, "radix point must be followed by digits");
               }
               //skip following digits
               do {
                  if (++curr >= end) { goto PUSH_NUM_TOK; }
               } while (mcsl::is_digit(*curr, radix));

            }
            //radix separator
            if (*curr == RADIX_SEPARATOR) {
               isReal = true;
               if (++curr >= end) { throwError(ErrCode::BAD_LITERAL, "radix separator must be followed by digits"); }
               //skip sign if present
               if (*curr == '-' || *curr == '+') { if (++curr >= end) { throwError(ErrCode::BAD_LITERAL, "radix separator must be followed by digits"); }}
               //check first digit
               if (!mcsl::is_digit(*curr, radix)) { throwError(ErrCode::BAD_LITERAL, "radix separator must be followed by digits"); }
               //skip following digits
               do {
                  if (++curr >= end) { goto PUSH_NUM_TOK; }
               } while (mcsl::is_digit(*curr, radix));
            }

PUSH_NUM_TOK:
            if (curr < end && mcsl::is_letter(*curr)) {
               throwError(ErrCode::BAD_LITERAL, "identifier may not start with digit, and numeric literal must not be directly followed by identifier without separating whitespace");
            }

            //convert to number and push token to stream
            if (isReal) {
               toks.emplace_back(mcsl::str_to_real(tokBegin, curr, radix));
            } else {
               toks.emplace_back(mcsl::str_to_uint(tokBegin, curr, radix));
            }
            // ++curr;
            break;


         //IDENTIFIERS
         case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'X': case 'o': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'Y': case 'Z':
         case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'x': case 'O': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'y': case 'z':
         case '_': {
            while (++curr < end && (mcsl::is_digit(*curr, 36) || *curr == '_')) {}
            mcsl::raw_str_span name{tokBegin,curr};
            KeywordID id = ALL_KEYWORDS[name];
            if (+id) {
               toks.emplace_back(id);
            } else {
               toks.emplace_back(name);
            }
         } break;


         //EOS
         case EOS:
            toks.emplace_back(TokenType::EOS);
            ++curr;
            break;

         //ESCAPE CHAR
         case ESCAPE_CHAR:
            // toks.emplace_back(TokenType::ESC);
            curr += 2;
            break;

         //PREPROCESSOR
         case PREPROC_INIT:
            lexingPreprocStmt = true;
            toks.emplace_back(TokenType::PREPROC_INIT);
            ++curr;
            break;
         //NEWLINE
         case '\n':
            if (lexingPreprocStmt) {
               toks.emplace_back(TokenType::PREPROC_EOS);
               lexingPreprocStmt = false;
            }
            [[fallthrough]];
         //OTHER WHITESPACE
         case  ' ': case '\t': case '\v': case '\f': case '\r':
            ++curr;
            break;
         
         //OPERATORS
         case '!': case '$': case '%': case '&': case '+': case ',': case '-': case '.': case '=': case '?': case '@': case '^': case '|': case '~': case '(': case ')': case '[': case ']': case '{': case '}': case '*': case '/': case '<': case '>': case ':': {
            OpData op = OPERATORS[mcsl::raw_str_span{curr, end}];
            debug_assert(op);
            curr += op.size();
            switch (op.tokType()) {
               case TokenType::OP: toks.emplace_back(op); break;
               case TokenType::BLOCK_DELIM:
                  switch (op.opID()) {
                     case OpID::CALL_INVOKE: toks.emplace_back(BlockType::CALL, BlockDelimRole::OPEN); toks.emplace_back(BlockType::CALL, BlockDelimRole::CLOSE); break;
                     case OpID::CALL_OPEN  : toks.emplace_back(BlockType::CALL, BlockDelimRole::OPEN); break;
                     case OpID::CALL_CLOSE : toks.emplace_back(BlockType::CALL, BlockDelimRole::CLOSE); break;

                     case OpID::SUBSCRIPT_INVOKE: toks.emplace_back(BlockType::SUBSCRIPT, BlockDelimRole::OPEN); toks.emplace_back(BlockType::SUBSCRIPT, BlockDelimRole::CLOSE); break;
                     case OpID::SUBSCRIPT_OPEN  : toks.emplace_back(BlockType::SUBSCRIPT, BlockDelimRole::OPEN); break;
                     case OpID::SUBSCRIPT_CLOSE : toks.emplace_back(BlockType::SUBSCRIPT, BlockDelimRole::CLOSE); break;
                     
                     case OpID::LIST_INVOKE: toks.emplace_back(BlockType::INIT_LIST, BlockDelimRole::OPEN); toks.emplace_back(BlockType::INIT_LIST, BlockDelimRole::CLOSE); break;
                     case OpID::LIST_OPEN  : toks.emplace_back(BlockType::INIT_LIST, BlockDelimRole::OPEN); break;
                     case OpID::LIST_CLOSE : toks.emplace_back(BlockType::INIT_LIST, BlockDelimRole::CLOSE); break;

                     case OpID::SPECIALIZER_INVOKE: toks.emplace_back(BlockType::SPECIALIZER, BlockDelimRole::OPEN); toks.emplace_back(BlockType::SPECIALIZER, BlockDelimRole::CLOSE); break;
                     case OpID::SPECIALIZER_OPEN  : toks.emplace_back(BlockType::SPECIALIZER, BlockDelimRole::OPEN); break;
                     case OpID::SPECIALIZER_CLOSE : toks.emplace_back(BlockType::SPECIALIZER, BlockDelimRole::CLOSE); break;

                     default: UNREACHABLE;
                  }
                  break;
               case TokenType::PTXT_SEG: //comments (strings and chars handled separately)
                  switch (op.opID()) {
                     case OpID::BLOCK_CMNT: break;
                     case OpID::BLOCK_CMNT_OPEN: 
                        do {
                           if (curr >= end) { throwError(ErrCode::BAD_CMNT, "unclosed block comment"); }
                           OpData tmp = OPERATORS[mcsl::raw_str_span{curr,end}];
                           if (tmp == OpID::BLOCK_CMNT_CLOSE) { curr += tmp.size(); break; }
                           ++curr;
                        } while (true);
                        break;
                     case OpID::BLOCK_CMNT_CLOSE: throwError(ErrCode::BAD_CMNT, "floating block comment closing delimiter");

                     case OpID::LINE_CMNT:
                        while (curr < end) {
                           if (*curr == '\n') { ++curr; break; }
                           if (*curr == '\\') { ++curr; }
                           ++curr;
                        }
                        break;

                     default: UNREACHABLE;
                  }
                  break;

               default: UNREACHABLE;
            }

         } break;

         //CHARS
         case CHAR_DELIM: {
            ++curr; //skip opening quote
            const char tmp = parseChar(curr, end);
            if (curr >= end || *curr != CHAR_DELIM) { throwError(ErrCode::BAD_LITERAL, "character literal may only contain a single character/escape sequence"); }
            ++curr; //skip closing quote
            toks.emplace_back(tmp);
         } break;

         //STRINGS
         case STR_DELIM:
            while (++curr < end && *curr != STR_DELIM) {
               if (*curr == ESCAPE_CHAR) {
                  ++curr;
               }
            }
            if (curr >= end || *curr != STR_DELIM) {
               throwError(ErrCode::BAD_LITERAL, "unclosed string literal");
            }
            ++curr;
            toks.emplace_back(mcsl::raw_str_span{tokBegin,curr}, PtxtType::UNPROCESSED_STR);
            break;

         //INTERPOLATED STRINGS
         case INTERP_STR_DELIM:
            while (++curr < end && *curr != INTERP_STR_DELIM) {
               if (*curr == ESCAPE_CHAR) {
                  ++curr;
                  continue;
               }
               if (*curr == PLACEHOLDER_INIT) {
                  ++curr;
                  if (curr < end) {
                     if (*curr == PLACEHOLDER_OPEN) {
                        //!NOTE: UNFINISHED (can't properly support expressions containing curly brackets)
                        while (++curr < end && *curr != PLACEHOLDER_CLOSE) {
                           lexExpr(curr, end, toks);
                        }
                        if (curr >= end) {
                           throwError(ErrCode::BAD_LITERAL, "unclosed interpolated string literal");
                        }
                     }
                  }
               }
            }
            if (curr >= end || *curr != INTERP_STR_DELIM) {
               throwError(ErrCode::BAD_LITERAL, "unclosed interpolated string literal");
            }
            ++curr;
            toks.emplace_back(mcsl::raw_str_span{tokBegin,curr}, PtxtType::UNPROCESSED_STR);
            break;
         
         //UNPRINTABLE CHAR (illegal)
         default:
            debug_assert(*curr < 32 || *curr > 126);
            throwError(ErrCode::LEXER_UNSPEC, "invalid character (%hhu)", *curr);
      }
   }

   return tokens;
}

//!lex a character literal
//!NOTE: CURRENTLY ONLY SUPPORTS ASCII CHARACTER LITERALS
char clef::Lexer::parseChar(char*& curr, char* const end) {
   char ch;
   if (*curr >= 32 && *curr <= 126) {
      if (*curr == ESCAPE_CHAR) {
         if (++curr >= end) { throwError(ErrCode::BAD_LITERAL, "incomplete character literal"); }
         switch (*curr) {
            #pragma region numescseq
            case 'b': case 'B': { //1-8 binary digits
               char* numBegin = ++curr;
               char* numEnd = numBegin + 8;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 2)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { throwError(ErrCode::BAD_LITERAL, "bad binary numeric escape sequence"); }
               ch = (char)mcsl::str_to_uint(numBegin, numEnd, 2);
               break;
            }
            case 'o': case 'O': { //1-3 octal digits
               //extract digit sequence
               char* numBegin = ++curr;
               char* numEnd = numBegin + 3;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 8)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { throwError(ErrCode::BAD_LITERAL, "bad octal numeric escape sequence"); }
               //parse digit sequence
               do { //!NOTE: kinda unsafe, but should alwawys work if implemented correctly
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
               char* numEnd = numBegin + 3;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 10)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { throwError(ErrCode::BAD_LITERAL, "bad decimal numeric escape sequence"); }
               //parse digit sequence
               do { //!NOTE: kinda unsafe, but should alwawys work if implemented correctly
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
               char* numEnd = numBegin + 2;
               while (curr < end && curr < numEnd) {
                  if (!mcsl::is_digit(*curr, 16)) {
                     numEnd = curr;
                     break;
                  }
                  ++curr;
               }
               if (numEnd != curr) { throwError(ErrCode::BAD_LITERAL, "bad hexidecimal numeric escape sequence"); }
               ch = (char)mcsl::str_to_uint(numBegin, numEnd, 16);
               break;
            }
            #pragma endregion numescseq

            // case 'a': ch = '\a'; ++curr; break;
            // case 'b': ch = '\b'; ++curr; break;
            case 'f': ch = '\f'; ++curr; break;
            case 'n': ch = '\n'; ++curr; break;
            case 'r': ch = '\r'; ++curr; break;
            case 't': ch = '\t'; ++curr; break;
            case 'v': ch = '\v'; ++curr; break;

            default: if (*curr < 32 || *curr > 126) { throwError(ErrCode::BAD_LITERAL, "unprintable escaped character literal (%hhu)", *curr); }
               //!NOTE: INCOMPLETE - needs a warning for unrecognized escape sequence
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
   } else { throwError(ErrCode::BAD_LITERAL, "unprintable character literal (%hhu)", *curr); }

   return ch;
}

bool clef::Lexer::lexExpr(char*& curr, char* const end, mcsl::dyn_arr<clef::Token>& toks) {
   throwError(ErrCode::LEXER_NOT_IMPLEMENTED, "lexExpr not yet implemented");
}

#endif //LEXER_CPP