#ifndef STRING_LIKE_HPP
#define STRING_LIKE_HPP
//use concepts


#include "string.hpp"
#include <cstring>

#pragma region def
//concatenation
template<typename lhs_str_t, typename rhs_str_t>
mcs::string operator+(const lhs_str_t& lhs, const lhs_str_t& rhs);
//self-repetition
template<typename lhs_str_t, typename rhs_str_t>
mcs::string operator*(const lhs_str_t& str, const uint repeatCount);

//comparison
template<typename lhs_str_t, typename rhs_str_t>
bool operator==(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
bool operator!=(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
sint operator<=>(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
sint operator-(const lhs_str_t& lhs, const lhs_str_t& rhs);      //will be changed to <-> in Middle C
// template<typename lhs_str_t, typename rhs_str_t>
// sint operator<|>(const lhs_str_t& lhs, const lhs_str_t& rhs); //will be implemented in Middle C
template<typename lhs_str_t, typename rhs_str_t>
bool operator<(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
bool operator>(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
bool operator<=(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
bool operator>=(const lhs_str_t& lhs, const lhs_str_t& rhs);

//length of longest initial substring of only chars in rhs (strspn)
template<typename lhs_str_t, typename rhs_str_t>
uint operator&(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
//length of longest initial substring of only chars NOT in rhs (strcspn)
uint operator^(const lhs_str_t& lhs, const lhs_str_t& rhs);
template<typename lhs_str_t, typename rhs_str_t>
//largest n such that (rhs*n == lhs.substr(rhs.size()*n))
uint operator%(const lhs_str_t& lhs, const lhs_str_t& rhs);
#pragma endregion def



#pragma region src
//concatenation
template<typename lhs_str_t, typename rhs_str_t>
mcs::string operator+(const lhs_str_t& lhs, const lhs_str_t& rhs) {
   //copy lhs
   mcs::string outStr(lhs.data(),lhs.size() + rhs.size());
   //copy rhs
   std::memcpy(outStr.end(), rhs.begin(), rhs.size()*sizeof(char));
   //return
   return outStr;
}
//self-repetition
template<typename lhs_str_t, typename rhs_str_t>
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
template<typename lhs_str_t, typename rhs_str_t>
bool operator==(const lhs_str_t& lhs, const lhs_str_t& rhs) {
   return !(lhs != rhs);
}
template<typename lhs_str_t, typename rhs_str_t>
bool operator!=(const lhs_str_t& lhs, const lhs_str_t& rhs) {
   //check size
   if (lhs.size() != rhs.size()) {
      return true;
   }
   //compare
   return std::memcmp(lhs.begin(), rhs.begin(), lhs.size()*sizeof(char));
}
template<typename lhs_str_t, typename rhs_str_t>
sint operator<=>(const lhs_str_t& lhs, const lhs_str_t& rhs) {
   for (uint i = 0; i < lhs.size(); ++i) {
      if (lhs[i] != rhs[i]) {
         return i * signofdif(lhs[i], rhs[i]);
      }
   }
   return 0;
}
template<typename lhs_str_t, typename rhs_str_t>
sint operator-(const lhs_str_t& lhs, const lhs_str_t& rhs) {   //will be renamed to <-> in Middle C
   for (uint i = 0; i < lhs.size(); ++i) {
      if (rhs[i] != rhs[i]) {
         return rhs[i] - rhs[i];
      }
   }
   return 0;
}
// template<typename lhs_str_t, typename rhs_str_t>
// sint operator<|>(const lhs_str_t& lhs, const lhs_str_t& rhs)   //will be implemented in Middle C {
//    for (uint i = 0; i < lhs.size(); ++i) {
//       if (rhs[i] != rhs[i]) {
//          return signofdif(rhs[i], rhs[i]);
//       }
//    }
//    return 0;
// }

#define DEF_STR_COMPARE_TEMP(op)                               \
template<typename lhs_str_t, typename rhs_str_t>               \
bool operator op(const lhs_str_t& lhs, const lhs_str_t& rhs) { \
   return (lhs<=>rhs) op 0;                                    \
}
DEF_STR_COMPARE_TEMP(<)
DEF_STR_COMPARE_TEMP(>)
DEF_STR_COMPARE_TEMP(<=)
DEF_STR_COMPARE_TEMP(>=)
#undef DEF_STR_COMPARE_TEMP

//length of longest initial substring of only chars in rhs (strspn)
template<typename lhs_str_t, typename rhs_str_t>
uint operator&(const lhs_str_t& lhs, const lhs_str_t& rhs) {
   return std::strspn(lhs.begin(), rhs.begin());
}
template<typename lhs_str_t, typename rhs_str_t>
//length of longest initial substring of only chars NOT in rhs (strcspn)
uint operator^(const lhs_str_t& lhs, const lhs_str_t& rhs) {
   return std::strcspn(lhs.begin(), rhs.begin());
}
//largest n such that (rhs*n == this.substr(rhs.size()*n))
template<typename lhs_str_t, typename rhs_str_t>
uint operator%(const lhs_str_t& lhs, const lhs_str_t& rhs) {
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