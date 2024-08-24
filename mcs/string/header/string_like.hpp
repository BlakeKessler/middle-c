#ifndef STRING_LIKE_HPP
#define STRING_LIKE_HPP
//!NOTE: use concepts


#include "string.hpp"
#include "raw_str_span.hpp"
#include "dyn_str_span.hpp"
#include "raw_str.hpp"
#include "raw_buf_str.hpp"
#include <cstring>
#include <concepts>

#pragma region types

template<typename T> concept string_like = requires(T a) {
   T::string_like == true;
};

struct mcs::str_hash_func {
   using hash_func_t = struct {};
   using is_transparent = void;

   //!NOTE: INCOMPLETE
};

#pragma endregion types

#pragma region cdef
//concatenation
template<string_like lhs_str_t>
mcs::string operator+(const lhs_str_t& lhs, const char* rhs);

//comparison
template<string_like lhs_str_t>
constexpr bool operator!=(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
constexpr bool operator==(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
sint operator-(const lhs_str_t& lhs, const char* rhs);      //will be changed to <-> in Middle C
// template<string_like lhs_str_t, string_like rhs_str_t>
// sint operator<|>(const lhs_str_t& lhs, const char* rhs); //will be implemented in Middle C

//length of longest initial substring of only chars in rhs (strspn)
template<string_like lhs_str_t>
uint operator&(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
//length of longest initial substring of only chars NOT in rhs (strcspn)
uint operator^(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
//largest n such that (rhs*n == lhs.substr(rhs.size()*n))
uint operator%(const lhs_str_t& lhs, const char* rhs);
#pragma endregion cdef

#pragma region csrc
//concatenation
template<string_like lhs_str_t>
mcs::string operator+(const lhs_str_t& lhs, const char* rhs) {
   const uint len = std::strlen(rhs);
   //copy lhs
   mcs::string outStr(lhs.data(),lhs.size() + len);
   //copy rhs
   std::memcpy(outStr.end(), rhs, len*sizeof(char));
   //return
   return outStr;
   
}

//comparison
template<string_like lhs_str_t>
constexpr bool operator!=(const lhs_str_t& lhs, const char* rhs) {
   //check size
   const uint len = std::strlen(rhs);
   if (lhs.size() != len) {
      return true;
   }
   //compare
   return std::memcmp(lhs.begin(), rhs, lhs.size()*sizeof(char));
}
template<string_like lhs_str_t>
constexpr bool operator==(const lhs_str_t& lhs, const char* rhs) {
   return !(lhs != rhs);
}
template<string_like lhs_str_t>
sint operator-(const lhs_str_t& lhs, const char* rhs);      //will be changed to <-> in Middle C
// template<string_like lhs_str_t, string_like rhs_str_t>
// sint operator<|>(const lhs_str_t& lhs, const char* rhs); //will be implemented in Middle C
template<string_like lhs_str_t>
bool operator<(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
bool operator>(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
bool operator<=(const lhs_str_t& lhs, const char* rhs);
template<string_like lhs_str_t>
bool operator>=(const lhs_str_t& lhs, const char* rhs);

//length of longest initial substring of only chars in rhs (strspn)
template<string_like lhs_str_t>
uint operator&(const lhs_str_t& lhs, const char* rhs) {
   return std::strspn(lhs.begin(),rhs);
}
template<string_like lhs_str_t>
//length of longest initial substring of only chars NOT in rhs (strcspn)
uint operator^(const lhs_str_t& lhs, const char* rhs) {
   return std::strcspn(lhs.begin(),rhs);
}
template<string_like lhs_str_t>
//largest n such that (rhs*n == lhs.substr(rhs.size()*n))
uint operator%(const lhs_str_t& lhs, const char* rhs) {
   const uint len = std::strlen(rhs);
   if (lhs.size() < len) {
      return 0;
   }
   uint modulus = 0;
   while (
      (len * (modulus + 1) > lhs.size()) &&
      !(std::memcmp(lhs.begin() + modulus * lhs.size(), rhs, len*sizeof(char)))
      ) {
         ++modulus;
   }
   return modulus;
}
#pragma endregion csrc





#pragma region def
//concatenation
template<string_like lhs_str_t, string_like rhs_str_t>
mcs::string operator+(const lhs_str_t& lhs, const rhs_str_t& rhs);
//self-repetition
template<string_like lhs_str_t>
mcs::string operator*(const lhs_str_t& str, const uint repeatCount);

//comparison
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator==(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator!=(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr sint operator<=>(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
sint operator-(const lhs_str_t& lhs, const rhs_str_t& rhs);      //will be changed to <-> in Middle C
// template<string_like lhs_str_t, string_like rhs_str_t>
// sint operator<|>(const lhs_str_t& lhs, const rhs_str_t& rhs); //will be implemented in Middle C
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator<(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator>(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator<=(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator>=(const lhs_str_t& lhs, const rhs_str_t& rhs);

//length of longest initial substring of only chars in rhs (strspn)
template<string_like lhs_str_t, string_like rhs_str_t>
uint operator&(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
//length of longest initial substring of only chars NOT in rhs (strcspn)
uint operator^(const lhs_str_t& lhs, const rhs_str_t& rhs);
template<string_like lhs_str_t, string_like rhs_str_t>
//largest n such that (rhs*n == lhs.substr(rhs.size()*n))
uint operator%(const lhs_str_t& lhs, const rhs_str_t& rhs);
#pragma endregion def








#pragma region src
//concatenation
template<string_like lhs_str_t, string_like rhs_str_t>
mcs::string operator+(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   //copy lhs
   mcs::string outStr(lhs.data(),lhs.size() + rhs.size());
   //copy rhs
   std::memcpy(outStr.end(), rhs.begin(), rhs.size()*sizeof(char));
   //return
   return outStr;
}
//self-repetition
template<string_like lhs_str_t>
mcs::string operator*(const lhs_str_t& str, const uint repeatCount) {
   mcs::string outStr{};
   outStr.realloc_exact(repeatCount * str.size());
   for (uint pos = 0; pos < outStr.size(); pos+=str.size()) {
      std::memcpy(outStr.begin() + pos, str.begin(), str.size()*sizeof(char));
   }
   *(outStr.end()) = '\0';
   return outStr;
}

//comparison
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator==(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   return !(lhs != rhs);
}
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr bool operator!=(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   //check size
   if (lhs.size() != rhs.size()) {
      return true;
   }
   //compare
   return std::memcmp(lhs.begin(), rhs.begin(), lhs.size()*sizeof(char));
}
template<string_like lhs_str_t, string_like rhs_str_t>
constexpr sint operator<=>(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   for (uint i = 0; i < lhs.size(); ++i) {
      if (lhs[i] != rhs[i]) {
         return i * signofdif(lhs[i], rhs[i]);
      }
   }
   return 0;
}
template<string_like lhs_str_t, string_like rhs_str_t>
sint operator-(const lhs_str_t& lhs, const rhs_str_t& rhs) {   //will be renamed to <-> in Middle C
   for (uint i = 0; i < lhs.size(); ++i) {
      if (rhs[i] != rhs[i]) {
         return rhs[i] - rhs[i];
      }
   }
   return 0;
}

#define DEF_STR_COMPARE_TEMP(op)                               \
template<string_like lhs_str_t, string_like rhs_str_t>               \
constexpr bool operator op(const lhs_str_t& lhs, const rhs_str_t& rhs) { \
   return (lhs<=>rhs) op 0;                                    \
}
DEF_STR_COMPARE_TEMP(<)
DEF_STR_COMPARE_TEMP(>)
DEF_STR_COMPARE_TEMP(<=)
DEF_STR_COMPARE_TEMP(>=)
#undef DEF_STR_COMPARE_TEMP

//length of longest initial substring of only chars in rhs (strspn)
template<string_like lhs_str_t, string_like rhs_str_t>
uint operator&(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   return std::strspn(lhs.begin(), rhs.begin());
}
template<string_like lhs_str_t, string_like rhs_str_t>
//length of longest initial substring of only chars NOT in rhs (strcspn)
uint operator^(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   return std::strcspn(lhs.begin(), rhs.begin());
}
//largest n such that (rhs*n == this.substr(rhs.size()*n))
template<string_like lhs_str_t, string_like rhs_str_t>
uint operator%(const lhs_str_t& lhs, const rhs_str_t& rhs) {
   if (lhs.size() < rhs.size()) {
      return 0;
   }
   uint modulus = 0;
   while (
      (rhs.size() * (modulus + 1) > lhs.size()) &&
      !(std::memcmp(lhs.begin() + modulus * lhs.size(), rhs.begin(), rhs.size()*sizeof(char)))
      ) {
         ++modulus;
   }
   return modulus;
}
#pragma endregion src

#endif //STRING_LIKE_HPP