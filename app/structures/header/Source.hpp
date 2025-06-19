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
      const mcsl::str_slice _path;
   public:
      Source():_buf(),_lines(),_path() {}
      Source(Source&& other):_buf(std::move(other._buf)),_lines(std::move(other._lines)),_path{other._path} { if (this != &other) { other.release(); } }
      Source(mcsl::string&& buf,mcsl::array<mcsl::str_slice>&& lines, mcsl::str_slice path = {}):_buf(std::move(buf)),_lines(std::move(lines)),_path{path} { buf.release(); lines.release(); }
      void release() {_buf.release();_lines.release();}
      
      static Source readFile(const char* path);

      mcsl::string& buf() { return _buf; }
      const mcsl::string& buf() const { return _buf; }
      mcsl::str_slice& line(const uint i) { return _lines[i]; }
      const mcsl::str_slice& line(const uint i) const { return _lines[i]; }
      mcsl::array<mcsl::str_slice>& lineArr() { return _lines; }
      uint lineCount() const { return _lines.size(); }
      const mcsl::str_slice path() const { return _path; }
};

#endif //SOURCE_HPP