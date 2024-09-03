#pragma once
#ifndef MCSL_STR_BASE_HPP
#define MCSL_STR_BASE_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "char_type.hpp"
#include "dyn_arr.hpp"


namespace mcsl{
   template<typename T, typename char_t> concept str_t = requires (T obj) { std::is_base_of_v<str_base<char_t>,T>; { obj.size() } -> std::convertible_to<luint>; };
}
#define CLEF_STR_T template<typename strT> requires str_t<strT,char_t>

//!IMPLEMENTATION GUIDE:
//!   consider null-termination
//!      if possible, owning string classes should maintain null-termination
//!      if the implementation doesn't ensure null-termination, DOCUMENT THAT   
//!   implement contig_base
//!   implement name
//!   write constructors
template<typename char_t>
struct mcsl::str_base : public contig_base<char_t> {
   //operations
   CLEF_STR_T strT copy(this const strT&& obj);

   // str_t& operator+=(const str_base& other);
   auto operator+(this const auto&& obj, const auto& other);
   
   // str_t& operator*=(const uint i);
   CLEF_STR_T strT operator*(this strT&& obj, const uint i);
   
   CLEF_STR_T strT&& alter(this strT&& obj, char (*const transformer)(const char));
   CLEF_STR_T strT altered(this const strT&& obj, char (*const transformer)(const char));
   
   inline auto&& operator+() { return alter(mcsl::to_upper); }
   inline auto&& operator-() { return alter(mcsl::to_lower); }
   inline auto as_upper() const { return altered(mcsl::to_upper); }
   inline auto as_lower() const { return altered(mcsl::to_lower); }

   uint operator&(this const auto&& obj, const auto& other); //strspn
   uint operator^(this const auto&& obj, const auto& other); //strcspn
   uint operator/(this const auto&& obj, const auto& other); //largest n : other*n ⊇ self

   CLEF_STR_T dyn_arr<strT> tokenize(this const strT&& obj, const str_base& delimChars);
   CLEF_STR_T dyn_arr<strT> tokenize(this const strT&& obj, const container_base<str_base>& delimStrs);

   //comparison
   CLEF_STR_T inline constexpr bool operator==(this auto&& s, const strT& other) { if (s.size() != other.size()) { return false; } return !(s <=> other); }
   CLEF_STR_T inline constexpr bool operator!=(this auto&& s, const strT& other) { if (s.size() != other.size()) { return true;  } return   s <=> other;  }

   CLEF_STR_T inline constexpr sint operator<=>(this auto&& s, const strT& other) { return s.strcmp(other); }


   template<uint len> inline constexpr bool operator==(this auto&& s, const char_t other[len]) { return s == raw_str_span(other); }
   template<uint len> inline constexpr bool operator!=(this auto&& s, const char_t other[len]) { return s != raw_str_span(other); }

   template<uint len> inline constexpr sint operator<=>(this auto&& s, const char_t other[len]) { return s <=> raw_str_span(other); }


   CLEF_STR_T constexpr sint strcmp(this auto&& s, const strT& other) {
      uint len = s.size() < other.size() ? s.size() : other.size();
      for (uint i = 0; i < len; ++i) {
         if (s[i] != other[i]) {
            return s[i] - other[i];
         }
      }

      //reached end of string
         //can't just be a subtraction of the bytes b/c non-null-terminated to support non-null-terminated strings 
      switch (signofdif(s.size(), other.size())) {
         case  0: return 0; break;
         case  1: return s[len]; break;
         case -1: return -other[len]; break;
         default: break;
      }
      return 0;
   }
   CLEF_STR_T constexpr sint substrcmp(this auto&& s, const strT& other) {
      uint len = s.size() < other.size() ? s.size() : other.size();
      for (uint i = 0; i < len; ++i) {
         if (s[i] != other[i]) {
            return s[i] - other[i];
         }
      }
      return 0;
   }

   //typecasts
   CLEF_STR_T inline constexpr operator bool(this strT&& obj) { return obj.size(); }
   CLEF_STR_T inline constexpr operator const char_t*(this const strT&& obj) { return obj.data(); }
   CLEF_STR_T inline constexpr operator char_t*(this strT&& obj) { return obj.data(); }
};

#undef CLEF_STR_T

#include "raw_str_span.hpp"

#endif //MCSL_STR_BASE_HPP