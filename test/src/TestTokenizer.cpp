#ifndef TOKENIZER_TEST
#define TOKENIZER_TEST

#include "Tokenizer.hpp"
#include "SourceReader.hpp"
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
   clef::StrView* linesArr;
   char* fileContents;
   const uint lineCount = clef::SourceReader::readFile(argv[1], linesArr, fileContents);

   //print file contents
   char fileDelim[] = "----------------------\n";
   std::printf("Printing \"%s\" (%u lines, %lu chars)\n%s", argv[1], lineCount, std::strlen(fileContents), fileDelim);
   for (uint i = 0; i < lineCount; ++i) {
      std::printf("line %u: %.*s", i + 1, linesArr[i].size(), linesArr[i].data());
   }
   printf("\n%sDone printing file.\n", fileDelim);


   //tokenize
   clef::Tokenizer tokenizer(linesArr, lineCount);

   //print tokens
   printf("\nPrinting tokens (%u tokens)\n%s", tokenizer.size(), fileDelim);
   tokenizer.printf();
   printf("%sDone printing tokens.\n", fileDelim);

   //free memory
   delete[] fileContents;
   delete[] linesArr;
}

#endif //TOKENIZER_TEST