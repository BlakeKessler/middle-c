#ifndef LEXER_CPP
#define LEXER_CPP

#include "Lexer.hpp"
#include "Source.hpp"
#include "TokenData.hpp"
#include "OperatorData.hpp"

#include "char_type.hpp"
// #include <cstring>
// #include <cstdio>

//!creates a SourceTokens from results of file reader
clef::SourceTokens clef::Lexer::LexSource(Source&& src) {
   SourceTokens tokens(std::move(src));
   auto& toks = tokens.tokens();
   auto& tokLines = tokens.lines();

   mcsl::array<mcsl::raw_str_span>& srcLines = tokens.source().lineArr();
   
   for (uint lineIndex = 0; lineIndex < srcLines.size(); ++lineIndex) {
      mcsl::raw_str_span& line = srcLines[lineIndex];
      char* curr = line.begin();
      char *const lineEnd = line.end();
      
      //process tokens in line
      while (
         //skip leading whitespace
         [&](){while(curr < lineEnd && !+tokType(*curr)) {++curr;} return curr < lineEnd;}()) {
            //init
            TokenType type = TokenType::ANY; //will contain the type of the token when finished
            TokenType workingType;
            char* tokStart = curr;

            //find end of token
            while ((curr < lineEnd) &&
               ((workingType = type & tokType(*curr)), +workingType)) {
                  type = workingType;
                  ++curr;
            };

            //emplace token
            if (tokStart != curr) {
               assert(curr < lineEnd);
               if (+(type & TokenType::OPLIKE)) {  //needs to be partitioned
                  emplaceOps(toks, {tokStart, curr});
               } else {                            //does not need to be partitioned
                  toks.emplace_back(tokStart, curr, type);
               }
            }
      }
      
      //emplace line
      const uint startIndex = lineIndex ? (tokLines[lineIndex-1].end() - toks.data()) : 0;
      tokLines.emplace(lineIndex, toks.ptr_to_buf(), startIndex, toks.size() - startIndex);
   }

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
      OpGroup group = ALL_OPLIKES[tokstr];

      //unable to deduce operator - part of invalid operator block
      //!NOTE: needs to be accounted for because they only cause syntax errors if not inside of plaintext blocks
      if (!group) {
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
      toks.emplace_back(tokstr.begin(), group.toString().size(), group.tokType());
      tokstr.inc_begin(group.toString().size());
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