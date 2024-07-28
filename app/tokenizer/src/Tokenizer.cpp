#ifndef TOKENIZER_CPP
#define TOKENIZER_CPP

#include "Tokenizer.hpp"
#include "MidC_Data.hpp"
#include "Source.hpp"
#include <cstring>
#include <cstdio>

//!null constructor
clef::Tokenizer::Tokenizer():
_src(),_tokens(),_tokLines() {
   
}
//!constructor from results of file reader
clef::Tokenizer::Tokenizer(Source& src):
_src(src),_tokens(),_tokLines(src.lineCount()) {
   mcs::raw_str_span line;
   uint counter;
   
   for (uint i = 0; i < src.lineCount(); ++i) {
      //initialize
      line = src.line(i);
      counter = 0;

      //skip leading whitespace
      while (line.size() && !+tokTypeArr[+line[0]]) { line.inc_begin(1); }

      //process line
      while (line.size()) {
         //find bounds of token
         counter = Token::findTokEnd(line);
         //add to array of tokens
         _tokens.emplace_back(line.begin(), counter, i);
         line.inc_begin(counter);
         
         //partition token if it is multiple operators
         if (_tokens.back().isType(TokenType::DLIM)) {
            while (counter = _tokens.back().maxOpLen(), counter && counter < _tokens.back().size()) {
               _tokens.emplace_back(_tokens.back().split(counter));
            }
         }

         //skip leading whitespace
         while (line.size() && !+tokTypeArr[+line[0]]) { line.inc_begin(1); }
      }
      //create token line & increment overall count
      _tokLines.emplace(i, _tokens.ptrToBuf(), mcs::pair{i?_tokLines[i-1].lastIndex():0, _tokens.size()});
   }
   src.release();
}

//!read and tokenize file
clef::Tokenizer clef::Tokenizer::TokenizeFile(const char* path) {
   Source src = Source::readFile(path);
   return Tokenizer{src};
}

//!formatted print of tokenizer
void clef::Tokenizer::printf() const {
   for (uint lineNum = 0; lineNum < _tokLines.size(); ++lineNum) {
      std::printf("line %u:", lineNum + 1);
      for (const Token& tok : _tokLines[lineNum]) {
         std::printf("\033[24m \033[4m");
         tok.printf();
      }
      std::printf("\033[24m\n");
   }
   return;
}

#endif //TOKENIZER_CPP