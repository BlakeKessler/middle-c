#include "SourceReader.hpp"
#include <cstdio>
#include <cstring>

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
   char fileDelim[] = "----------------------";
   std::printf("Printing \"%s\" (%u lines, %lu chars)\n%s\n", argv[1], lineCount, std::strlen(fileContents), fileDelim);
   for (uint i = 0; i < lineCount; ++i) {
      std::printf("line %u: %.*s", i + 1, linesArr[i].size(), linesArr[i].data());
   }
   printf("\n%s\nDone Printing\n", fileDelim);

   delete[] fileContents;
   delete[] linesArr;
}