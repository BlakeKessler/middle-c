#pragma once
#ifndef SOURCE_HPP
#define SOURCE_HPP

#include "CLEF.hpp"
#include "str_slice.hpp"
#include "array.hpp"
#include "string.hpp"

class clef::Source {
   private:
      mcsl::string _buf;
      mcsl::array<mcsl::str_slice> _lines;
   public:
      Source():_buf(),_lines() {}
      Source(Source&& other):_buf(std::move(other._buf)),_lines(std::move(other._lines)) { if (this != &other) { other.release(); } }
      Source(mcsl::string&& buf,mcsl::array<mcsl::str_slice>&& lines):_buf(std::move(buf)),_lines(std::move(lines)) { buf.release(); lines.release(); }
      void release() {_buf.release();_lines.release();}
      
      static Source readFile(const char* path);

      mcsl::string& buf() { return _buf; }
      const mcsl::string& buf() const { return _buf; }
      mcsl::str_slice& line(const uint i) { return _lines[i]; }
      const mcsl::str_slice& line(const uint i) const { return _lines[i]; }
      mcsl::array<mcsl::str_slice>& lineArr() { return _lines; }
      uint lineCount() const { return _lines.size(); }
};

#endif //SOURCE_HPP