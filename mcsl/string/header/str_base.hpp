#pragma once
#ifndef MCSL_STR_BASE_HPP
#define MCSL_STR_BASE_HPP

#include "MCSL.hpp"
#include "contig_base.hpp"
#include "char_type.hpp"
#include "dyn_arr.hpp"

#include <cstdio>

//!IMPLEMENTATION GUIDE:
//!   consider null-termination
//!      if possible, owning string classes should maintain null-termination
//!      if the implementation doesn't ensure null-termination, DOCUMENT THAT   
//!   implement contig_base
//!   implement name
//!   write constructors
template<typename char_t>
struct mcsl::str_base : public contig_base<char_t> {
   // static constexpr const raw_str _name = "str_base";
   static constexpr const char _name[] = "str_base";
   constexpr static const auto& name() { return _name; }
   void printf(this auto&& obj) { std::printf("%.*s", obj.size(), obj.begin()); }
   
   
   
   //operations
   template<str_t strT> strT copy(this const strT&& obj);

   // str_t& operator+=(const str_base& other);
   auto operator+(this const auto&& obj, const auto& other);
   
   // str_t& operator*=(const uint i);
   template<str_t strT> strT operator*(this strT&& obj, const uint i);
   
   template<str_t strT> strT&& alter(this strT&& obj, char (*const transformer)(const char));
   template<str_t strT> strT altered(this const strT&& obj, char (*const transformer)(const char));
   
   inline auto&& operator+() { return alter(mcsl::to_upper); }
   inline auto&& operator-() { return alter(mcsl::to_lower); }
   inline auto as_upper() const { return altered(mcsl::to_upper); }
   inline auto as_lower() const { return altered(mcsl::to_lower); }

   uint operator&(this const auto&& obj, const auto& other); //strspn
   uint operator^(this const auto&& obj, const auto& other); //strcspn
   uint operator/(this const auto&& obj, const auto& other); //largest n : other*n ⊇ self

   template<str_t strT> dyn_arr<strT> tokenize(this const strT&& obj, const str_base& delimChars);
   template<str_t strT> dyn_arr<strT> tokenize(this const strT&& obj, const container_base<str_base>& delimStrs);

   //comparison
   template<str_t strT> inline constexpr bool operator==(this auto&& s, const strT& other) { if (s.size() != other.size()) { return false; } return !(s <=> other); }
   template<str_t strT> inline constexpr bool operator!=(this auto&& s, const strT& other) { if (s.size() != other.size()) { return true;  } return   s <=> other;  }

   template<str_t strT> inline constexpr sint operator<=>(this auto&& s, const strT& other) { return s.strcmp(other); }


   template<uint len> inline constexpr bool operator==(this auto&& s, const char_t other[len]) { return s == raw_str_span(other); }
   template<uint len> inline constexpr bool operator!=(this auto&& s, const char_t other[len]) { return s != raw_str_span(other); }

   template<uint len> inline constexpr sint operator<=>(this auto&& s, const char_t other[len]) { return s <=> raw_str_span(other); }


   template<str_t strT> constexpr sint strcmp(this auto&& s, const strT& other) {
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
   template<str_t strT> constexpr sint substrcmp(this auto&& s, const strT& other) {
      uint len = s.size() < other.size() ? s.size() : other.size();
      for (uint i = 0; i < len; ++i) {
         if (s[i] != other[i]) {
            return s[i] - other[i];
         }
      }
      return 0;
   }

   //typecasts
   inline constexpr operator bool(this auto&& obj) { return obj.size(); }
   inline constexpr operator const char_t*(this const auto&& obj) { return obj.data(); }
   inline constexpr operator char_t*(this auto&& obj) { return obj.data(); }
};

#endif //MCSL_STR_BASE_HPP