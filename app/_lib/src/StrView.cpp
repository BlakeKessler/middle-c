#ifndef STR_VIEW_CPP
#define STR_VIEW_CPP

#include "StrView.hpp"
#include "StrUtils.hpp"

clef::StrView::StrView() {
   _str = nullptr;
   _size = 0;
}
clef::StrView::StrView(char* begin, const uint len) {
   _str = begin;
   _size = len;
}
clef::StrView::StrView(char* begin, char* end) {
   _str = begin;
   _size = end - begin;
}

//!check if an address is within the domain of the viewed substring
bool clef::StrView::contains(char* ch) const {
   return (ch >= _str) && (ch < (_str + _size));
}

//!safe element access
char clef::StrView::at(const uint i) const {
   if (i >= _size) {
      clef::throwError(ErrCode::SEGFAULT, "StrView of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m", _size, i);
   }
   return _str[i];
}
//!safe element access
char& clef::StrView::at(const uint i) {
   if (i >= _size) {
      clef::throwError(ErrCode::SEGFAULT, "StrView of size \033[4m%u\033[24m accessed at index \033[4m%u\033[24m", _size, i);
   }
   return _str[i];
}

//!set beginning of view
char* clef::StrView::setBegin(char* begin) {
   _str = begin;
   --_size;
   return begin;
}
//!set end of view
char* clef::StrView::setEnd(char* end) {
   _size = end - _str;
   return end;
}
//!set size of view
char* clef::StrView::setSize(const uint size) {
   _size = size;
   return _str + _size;
}
//!add to start of view
char* clef::StrView::incBegin(const sint dist) {
   _str += dist;
   _size -= dist;
   return _str;
}
//!add to end of view
char* clef::StrView::incEnd(const sint dist) {
   _size += dist;
   return _str + _size;
}
//!set size of view
char* clef::StrView::incSize(const sint dist) {
   if (-dist > (lsint)_size) {
      clef::throwError(ErrCode::SEGFAULT, "cannot set StrView size to %d", _size + dist);
   }
   _size += dist;
   return _str + _size;
}

//!compare viewed substrings
bool clef::StrView::operator==(const StrView& other) const {
   return (_size == other.size()) && (strcmp(_str, other._str, _size) == 0);
}
//!compare viewed substrings
bool clef::StrView::operator!=(const StrView& other) const {
   return (_size != other.size()) || (strcmp(_str, other._str, _size) != 0);
}
//!compare viewed substrings
bool clef::StrView::operator==(const char* other) const {
   return !strcmp(_str, other, _size);
}
//!compare viewed substrings
bool clef::StrView::operator!=(const char* other) const {
   return strcmp(_str, other, _size);
}


//print
void clef::StrView::printf() const {
   std::printf("%.*s",_size,_str);
}

#endif //STR_VIEW_CPP