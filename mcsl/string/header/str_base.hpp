#pragma once
#ifndef MCSL_STR_BASE_HPP
#define MCSL_STR_BASE_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "char_type.hpp"
#include "dyn_arr.hpp"

#define THIS static_cast<str_t*>(this)
#define const_THIS static_cast<const str_t*>(this)
#define SELF (*static_cast<str_t*>(this))
#define const_SELF (*static_cast<const str_t*>(this))

//!IMPLEMENTATION GUIDE:
//!   consider null-termination
//!      if possible, owning string classes should maintain null-termination
//!      if the implementation doesn't ensure null-termination, DOCUMENT IT   
//!   implement contig_base
//!   implement name
//!   write constructors
template<typename str_t, typename char_t>
struct mcsl::str_base : public contig_base<char_t, str_base<str_t,char_t>> {
   //operations
   str_t& copy() const;

   // str_t& operator+=(const str_base& other);
   template<typename s, typename c> str_t operator+(const str_base<s,c>& other);
   
   // str_t& operator*=(const uint i);
   str_t operator*(const uint i);
   
   str_t& alter(char (*const transformer)(const char));
   str_t altered(char (*const transformer)(const char)) const;
   
   inline str_t& operator+() { return alter(mcsl::to_upper); }
   inline str_t& operator-() { return alter(mcsl::to_lower); }
   inline str_t as_upper() const { return altered(mcsl::to_upper); }
   inline str_t as_lower() const { return altered(mcsl::to_lower); }

   template<typename s, typename c> uint operator&(const str_base<s,c>& other) const; //strspn
   template<typename s, typename c> uint operator^(const str_base<s,c>& other) const; //strcspn
   template<typename s, typename c> uint operator/(const str_base<s,c>& other) const; //largest n : other*n ⊇ self

   dyn_arr<const str_t> tokenize(const str_base& delimChars) const;
   template<typename s, typename c, typename ctnr_t = arr_span<str_base>> dyn_arr<const str_t> tokenize(const container_base<str_base<s,c>, ctnr_t>& delimStrs) const;

   //comparison
   template<typename s, typename c> inline constexpr bool operator==(const str_base<s,c>& other) const { if (const_SELF.size() != other.size()) { return false; } return !(self <=> other); }
   template<typename s, typename c> inline constexpr bool operator!=(const str_base<s,c>& other) const { if (const_SELF.size() != other.size()) { return true;  } return   self <=> other;  }

   template<typename s, typename c> inline constexpr bool operator<(const str_base<s,c>& other) const { return (self <=> other) < 0; }
   template<typename s, typename c> inline constexpr bool operator<=(const str_base<s,c>& other) const { return (self <=> other) <= 0; }

   template<typename s, typename c> inline constexpr bool operator>(const str_base<s,c>& other) const { return (self <=> other) > 0; }
   template<typename s, typename c> inline constexpr bool operator>=(const str_base<s,c>& other) const { return (self <=> other) >= 0; }

   template<typename s, typename c> inline constexpr sint operator<=>(const str_base<s,c>& other) const { return strcmp(self, other); }


   template<typename s, typename c> constexpr sint strcmp(const str_base<s,c>& other) const {
      uint len = const_SELF.size() < other.size() ? const_SELF.size() : other.size();
      for (uint i = 0; i < len; ++i) {
         if (self[i] != other[i]) {
            return self[i] - other[i];
         }
      }

      //reached end of string
         //can't just be a subtraction of the bytes b/c non-null-terminated to support non-null-terminated strings 
      switch (signofdif(const_SELF.size(), other.size())) {
         case  0: return 0; break;
         case  1: return self[len]; break;
         case -1: return -other[len]; break;
         default: break;
      }
      return 0;
   }
   template<typename s, typename c> constexpr sint substrcmp(const str_base<s,c>& other) const {
      uint len = SELF.size() < other.size() ? const_SELF.size() : other.size();
      for (uint i = 0; i < len; ++i) {
         if (self[i] != other[i]) {
            return self[i] - other[i];
         }
      }
      return 0;
   }

   //typecasts
   inline constexpr operator bool() const { return const_SELF.size(); }
   inline constexpr operator char*() { return const_SELF.data(); }
   inline constexpr operator const char*() const { return const_SELF.data(); }
};

#undef THIS
#undef const_THIS
#undef SELF
#undef const_SELF

#endif //MCSL_STR_BASE_HPP