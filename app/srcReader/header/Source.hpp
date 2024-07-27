#pragma once
#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "CLEF.hpp"
#include "StrView.hpp"
#include "array.hpp"

class clef::Source {
   private:
      mcs::array<char> _buf;
      mcs::array<StrView> _lines;
   public:
      Source():_buf(),_lines() {}
      Source(mcs::array<char>& buf,mcs::array<StrView>& lines):_buf(buf),_lines(lines) {}
      void free() const {_buf.free();_lines.free();}
      
      static Source readFile(const char* path);

      auto& buf() { return _buf; }
      auto& line(const uint i) { return _lines[i]; }
      auto& lineArr() { return _lines; }
      uint lineCount() const { return _lines.size(); }
};

#endif //SOURCE_HPP