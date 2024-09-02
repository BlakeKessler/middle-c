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
clef::Tokenizer::Tokenizer(Source&& src):
_src(std::move(src)),_tokens(),_tokLines(src.lineCount()) {
   src.release();
   _tokens.emplace_back();
   mcsl::raw_str_span line;
   uint tempCounter;
   
   for (uint i = 0; i < _src.lineCount(); ++i) {
      //initialize
      line = _src.line(i);
      tempCounter = 0;

      //skip leading whitespace
      while (line.size() && !+Token::typeNum(line[0],false)) { line.inc_begin(1); }

      //process line
      while (line.size()) {
         //find bounds of token
         tempCounter = Token::findTokEnd(line);
         //add to array of tokens
         _tokens.emplace_back(line.begin(), tempCounter, i);
         line.inc_begin(tempCounter);
         
         //partition token if it is multiple operators
         if (_tokens.back().isType(TokenType::DLIM)) {
            while (tempCounter = _tokens.back().maxOpLen(), tempCounter && tempCounter < _tokens.back().size()) {
               _tokens.emplace_back(_tokens.back().split(tempCounter));
            }
         }

         //skip leading whitespace
      while (line.size() && !+Token::typeNum(line[0],false)) { line.inc_begin(1); }
      }
      //create token line & increment overall count
      _tokLines.emplace(i, _tokens.ptr_to_buf(), mcsl::pair{i?_tokLines[i-1].last_index():0 , _tokens.size()});
   }
}

//!read and tokenize file
clef::Tokenizer clef::Tokenizer::TokenizeFile(const char* path) {
   Source src = Source::readFile(path);
   return Tokenizer{std::move(src)};
}

//!formatted print of tokenizer
void clef::Tokenizer::printf() const {
   for (uint lineNum = 0; lineNum < _tokLines.size(); ++lineNum) {
      std::printf("line %u:", lineNum + 1);
      for (const Token& tok : _tokLines[lineNum]) {
         std::printf("\033[24m \033[4m");
         tok.size() ? tok.printf(),0 : std::printf("\033[31mNIL\033[39m");
      }
      std::printf("\033[24m\n");
   }
   return;
}

#endif //TOKENIZER_CPP