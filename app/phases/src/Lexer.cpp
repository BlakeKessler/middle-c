#ifndef LEXER_CPP
#define LEXER_CPP

#include "Lexer.hpp"
#include "MidC_Data.hpp"
#include "Source.hpp"
#include <cstring>
#include <cstdio>

//!null constructor
clef::Lexer::Lexer():
_src(),_tokens(),_tokLines() {
   
}
//!constructor from results of file reader
clef::Lexer::Lexer(Source&& src):
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
//!move constructor
clef::Lexer::Lexer(Lexer&& other):
   _src(std::move(other._src)),
   _tokens(std::move(other._tokens)),
   _tokLines(std::move(other._tokLines)) {
      other.release();
}

//!read and tokenize file
clef::Lexer clef::Lexer::TokenizeFile(const char* path) {
   return Lexer{Source::readFile(path)};
}

//!formatted print of tokenizer
void clef::Lexer::printf() const {
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

#endif //LEXER_CPP