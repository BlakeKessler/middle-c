#ifndef SOURCE_CPP
#define SOURCE_CPP

#include "Source.hpp"
#include <cstdio>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

//!read Middle C source code file into vector of lines of code
//!returns a Source object with the data
clef::Source clef::Source::readFile(const char* path) {
   //get file size and allocate buffer
   //extra char allocated to ensure null-termination
   const uint fileSize = fs::file_size(fs::path(path)) + sizeof(char);

   mcs::array<char> _buf(fileSize);

   //open, read, close, back-fill file
   FILE* srcFile = std::fopen(path, "r");
   const uint charsRead = std::fread(_buf.begin(), sizeof(char), fileSize, srcFile);
   std::fclose(srcFile);
   std::memset(_buf.begin() + charsRead, '\0', fileSize - charsRead*sizeof(char));

   //find number of lines
   uint lineCount = 1;
   for (uint i = 0; i < charsRead; ++i) {
      lineCount += (_buf[i] == '\n');
   }
   //allocate array of lines
   mcs::array<StrView> _lines(lineCount);
   //find line breaks and fill array of lines
   char* current = _buf.begin();
   uint len;
   for (uint i = 0; i < lineCount; current += len,++i) {
      len = std::strcspn(current, "\n") + 1;
      _lines.emplace(i, current, len);
   }

   //return
   return Source(_buf,_lines);
}

#endif //SOURCE_CPP