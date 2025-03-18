#ifndef SOURCE_CPP
#define SOURCE_CPP

#include "Source.hpp"
#include "fs.hpp"
#include <filesystem>
#include "assert.hpp"

namespace fs = std::filesystem;

//!read Middle C source code file into vector of lines of code
//!returns a Source object with the data
//!NOTE: std::fgets to get a line
clef::Source clef::Source::readFile(const char* path) {
   //read file into a string
   const uint fileSize = fs::file_size(fs::path(path));
   mcsl::File srcFile(path, "r");
   mcsl::string _buf = srcFile.readChars(fileSize);
   srcFile.close();
   assert(fileSize == _buf.size(), "incorrect number of characters read from source code file");

   //validate file encoding (UTF-8)
   //!NOTE: UNFINISHED


   //find number of lines
   uint lineCount = 1;
   for (uint i = 0; i < _buf.size(); ++i) {
      lineCount += (_buf[i] == '\n');
   }
   //allocate array of lines
   mcsl::array<mcsl::str_slice> _lines(lineCount);
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