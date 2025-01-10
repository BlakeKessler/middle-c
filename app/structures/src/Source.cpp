#ifndef SOURCE_CPP
#define SOURCE_CPP

#include "Source.hpp"
#include <cstdio>
#include <filesystem>
#include "assert.hpp"

namespace fs = std::filesystem;

//!read Middle C source code file into vector of lines of code
//!returns a Source object with the data
//!NOTE: std::fgets to get a line
clef::Source clef::Source::readFile(const char* path) {
   //get file size and allocate buffer
   //extra char allocated to ensure null-termination
   const uint fileSize = fs::file_size(fs::path(path));

   mcsl::string _buf{fileSize};
   // _buf.resize_exact(fileSize);

   //open, read, close, back-fill file
   FILE* srcFile = std::fopen(path, "r");
   const uint charsRead = std::fread(_buf.begin(), 1, fileSize, srcFile);
   std::fclose(srcFile);
   assert(charsRead == _buf.size());

   //validate file encoding (UTF-8)
   //!NOTE: UNFINISHED


   //find number of lines
   uint lineCount = 1;
   for (uint i = 0; i < charsRead; ++i) {
      lineCount += (_buf[i] == '\n');
   }
   //allocate array of lines
   mcsl::array<mcsl::raw_str_span> _lines(lineCount);
   //find line breaks and fill array of lines
   char* current = _buf.begin();
   uint len;
   for (uint i = 0; i < lineCount; current += len, ++i) {
      len = std::strcspn(current, "\n") + 1;
      _lines.emplace(i, current, len);
   }

   //return
   return Source(std::move(_buf),std::move(_lines));
}

#endif //SOURCE_CPP