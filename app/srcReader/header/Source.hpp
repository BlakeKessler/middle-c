#pragma once
#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "CLEF.hpp"
#include "raw_str_span.hpp"
#include "array.hpp"
#include "string.hpp"

class clef::Source {
   private:
      mcsl::string _buf;
      mcsl::array<mcsl::raw_str_span> _lines;
   public:
      Source():_buf(),_lines() {}
      Source(Source&& other):_buf(other._buf),_lines(other._lines) { other.release(); }
      Source(mcsl::string&& buf,mcsl::array<mcsl::raw_str_span>&& lines):_buf(buf),_lines(lines) { buf.release(); lines.release(); }
      void free() const {_buf.free();_lines.free();}
      void release() {_buf.release();_lines.release();}
      
      static Source readFile(const char* path);

      mcsl::string& buf() { return _buf; }
      mcsl::raw_str_span& line(const uint i) { return _lines[i]; }
      mcsl::array<mcsl::raw_str_span>& lineArr() { return _lines; }
      uint lineCount() const { return _lines.size(); }
};

#endif //SOURCE_HPP