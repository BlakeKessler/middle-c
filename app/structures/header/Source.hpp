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

      static constexpr const mcsl::raw_str _nameof = "Source";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      Source():_buf(),_lines() {}
      Source(Source&& other):_buf(std::move(other._buf)),_lines(std::move(other._lines)) { if (this != &other) { other.release(); } }
      Source(mcsl::string&& buf,mcsl::array<mcsl::raw_str_span>&& lines):_buf(std::move(buf)),_lines(std::move(lines)) { buf.release(); lines.release(); }
      void release() {_buf.release();_lines.release();}
      
      static Source readFile(const char* path);

      mcsl::string& buf() { return _buf; }
      const mcsl::string& buf() const { return _buf; }
      mcsl::raw_str_span& line(const uint i) { return _lines[i]; }
      const mcsl::raw_str_span& line(const uint i) const { return _lines[i]; }
      mcsl::array<mcsl::raw_str_span>& lineArr() { return _lines; }
      uint lineCount() const { return _lines.size(); }
};

#endif //SOURCE_HPP