#ifndef LEXER_CPP
#define LEXER_CPP

#include "Lexer.hpp"
#include "MidC_Data.hpp"
#include "Source.hpp"

#include "char_type.hpp"
#include <cstring>
#include <cstdio>

//!constructor from results of file reader
clef::SourceTokens clef::Lexer::LexSource(Source&& src) {
   SourceTokens tokens(std::move(src));
   auto& toks = tokens.tokens();
   auto& tokLines = tokens.allRows();

   mcsl::array<mcsl::raw_str_span>& srcLines = tokens.source().lineArr();
   
   for (uint lineIndex = 0; lineIndex < srcLines.size(); ++lineIndex) {
      mcsl::raw_str_span& line = srcLines[lineIndex];
      char* curr = line.begin();
      char *const end = line.end();
      
      //process tokens in line
      while (
         //skip leading whitespace - in lambda to allow comma notation
         [&](){while(curr < end && !+tokType(*curr)) {++curr;}}(),
         //condition
         curr < end) {
            //init
            TokenType type = TokenType::ANY; //will contain the type of the token when finished
            TokenType workingType;
            char* tokStart = curr;

            //find end of token
            while ((curr < end) &&
               ((workingType = type & tokType(*curr)), +workingType)) {
                  type = workingType;
                  ++curr;
            };

            //emplace token
            if (tokStart != curr) {
               assert(curr < end);
               toks.emplace_back(tokStart, curr, type);
            }
      }
      
      //emplace line
      const uint startIndex = lineIndex ? (tokLines[lineIndex-1].end() - toks.data()) : 0;
      tokLines.emplace(lineIndex, toks.ptr_to_buf(), startIndex, toks.size() - startIndex);
   }

   return tokens;
}

#endif //LEXER_CPP