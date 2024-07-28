#pragma once
#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "CLEF.hpp"
#include "raw_str_span.hpp"
#include "array.hpp"
#include "string.hpp"

class clef::Source {
   private:
      mcs::string _buf;
      mcs::array<mcs::raw_str_span> _lines;
   public:
      Source():_buf(),_lines() {}
      Source(mcs::string& buf,mcs::array<mcs::raw_str_span>& lines):_buf(buf),_lines(lines) { buf.release(); lines.release(); }
      void free() const {_buf.free();_lines.free();}
      void release() {_buf.release();_lines.release();}
      
      static Source readFile(const char* path);

      auto& buf() { return _buf; }
      auto& line(const uint i) { return _lines[i]; }
      auto& lineArr() { return _lines; }
      uint lineCount() const { return _lines.size(); }
};

#endif //SOURCE_HPP