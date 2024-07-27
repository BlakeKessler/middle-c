#pragma once
#ifndef STR_VIEW_HPP
#define STR_VIEW_HPP

#include "CLEF.hpp"

class clef::StrView{
   protected:
      //string being viewed
      char* _str;
      //length of view
      uint _size;
   public:
      //constructors
      StrView();
      StrView(char* begin, const uint len);
      StrView(char* begin, char* end);

      //bounds
      uint size() const { return _size; }
      char* data() const { return _str; }
      char* begin() const { return _str; }
      char* end() const { return _str + _size; }
      bool contains(char* ch) const;

      //element access
      char operator[](const uint i) const { return _str[i]; }
      char& operator[](const uint i) { return _str[i]; }
      char at(const uint i) const;
      char& at(const uint i);

      //bounds manipulation
      char* setBegin(char* begin);
      char* setEnd(char* end);
      char* setSize(const uint size);
      char* incBegin(const sint dist);
      char* incEnd(const sint dist);
      char* incSize(const sint dist);

      //operators
      bool operator==(const StrView& other) const;
      bool operator!=(const StrView& other) const;
      bool operator==(const char* other) const;
      bool operator!=(const char* other) const;

      //io
      void printf() const;
};

#endif //STR_VIEW_HPP