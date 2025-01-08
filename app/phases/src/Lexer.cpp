#ifndef LEXER_CPP
#define LEXER_CPP

#include "Lexer.hpp"
#include "Source.hpp"
#include "TokenData.hpp"
#include "OperatorData.hpp"

#include "char_type.hpp"
#include "str_to_num.hpp"
// #include <cstring>
// #include <cstdio>

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

   assert(curr < end);
   do {
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
            ++curr;
            break;


         //IDENTIFIERS
         case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'X': case 'o': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'Y': case 'Z':
         case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'x': case 'O': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'y': case 'z':
         case '_':
            while (++curr < end && (mcsl::is_digit(*curr, 36) || *curr == '_')) {}
            //!NOTE: UNFINISHED (check for keywords)
            toks.emplace_back(mcsl::raw_str_span{tokBegin, curr});
            break;



         //!NOTE: UNFINISHED
         
      }
   } while (curr < end);

   return tokens;
}


//!helper function for creating operator-like tokens
//!   toks: tokens buffer to push to
//!   begin: beginning of token block
//!   end: end of token block
//!return value: number of operators from block
uint clef::Lexer::emplaceOps(mcsl::dyn_arr<Token>& toks, mcsl::raw_str_span tokstr) {
   //init
   uint opCount = 0;
   uint invalids = 0;
   //process operator block
   while (tokstr.size()) {
      //deduce operator
      OpData data = OPERATORS[tokstr];

      //unable to deduce operator - part of invalid operator block
      //!NOTE: needs to be accounted for because they only cause syntax errors if not inside of plaintext blocks
      if (!data) {
         ++invalids;
         tokstr.inc_begin(1);
         continue;
      }


      //handle invalid operator block if necessary
      if (invalids) {
         ++opCount;
         toks.emplace_back(tokstr.begin()-invalids, invalids, TokenType::NONE);
         invalids = 0;
      }

      //emplace token
      toks.emplace_back(tokstr.begin(), data.toString().size(), data.tokType());
      tokstr.inc_begin(data.toString().size());
      ++opCount;
   }


   //handle a trailing invalid operator block if necessary
   if (invalids) {
      toks.emplace_back(tokstr.begin()-invalids, invalids, TokenType::NONE);
      ++opCount;
   }

   //return
   return opCount;
}


#endif //LEXER_CPP