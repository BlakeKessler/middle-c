#ifndef TOKENIZER_TEST
#define TOKENIZER_TEST

#include "Lexer.hpp"

#include <cstring>
#include <string>
#include <sstream>
#include <iostream>

int main(int argc, char** argv) {
   //check that file was provided
   if (argc < 2) {
      std::printf("No file path provided\n");
      return -1;
   }

   //read file
   clef::Source src = clef::Source::readFile(argv[1]);

   //print file contents
   char fileDelim[] = "----------------------\n";
   std::printf("Printing \"%s\" (%u lines, %u chars)\n%s", argv[1], src.lineCount(), src.buf().size(), fileDelim);
   for (uint i = 0; i < src.lineCount(); ++i) {
      std::printf("line %u: %.*s", i + 1, src.line(i).size(), src.line(i).data());
   }
   printf("\n%sDone printing file.\n", fileDelim);


   //tokenize
   clef::SourceTokens tokens = clef::Lexer::LexSource(std::move(src));

   //print tokens
   printf("\nPrinting tokens (%u tokens)\n%s", tokens.size(), fileDelim);
   tokens.printf();
   printf("%sDone printing tokens.\n", fileDelim);
}

#endif //TOKENIZER_TEST