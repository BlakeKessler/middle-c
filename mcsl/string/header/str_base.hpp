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


//!IMPLEMENTATION GUIDE:
//!   consider null-termination
//!      if possible, owning string classes should maintain null-termination
//!      if the implementation doesn't ensure null-termination, DOCUMENT THAT   
//!   implement contig_base
//!   implement name
//!   write constructors
template<typename char_t>
struct mcsl::str_base : public contig_base<char_t> {
   // using contig_base<char_t>::size;
   // using contig_base<char_t>::data;
   // using contig_base<char_t>::ptr_to_buf;
   //operations
   template<typename Self_t> requires str_t<Self_t,char_t> Self_t copy(this const Self_t&& obj);

   // str_t& operator+=(const str_base& other);
   auto operator+(this const auto&& obj, const auto& other);
   
   // str_t& operator*=(const uint i);
   template<typename Self_t> requires str_t<Self_t,char_t> Self_t operator*(this Self_t&& obj, const uint i);
   
   template<typename Self_t> requires str_t<Self_t,char_t> Self_t&& alter(this Self_t&& obj, char (*const transformer)(const char));
   template<typename Self_t> requires str_t<Self_t,char_t> Self_t altered(this const Self_t&& obj, char (*const transformer)(const char));
   
   inline auto&& operator+() { return alter(mcsl::to_upper); }
   inline auto&& operator-() { return alter(mcsl::to_lower); }
   inline auto as_upper() const { return altered(mcsl::to_upper); }
   inline auto as_lower() const { return altered(mcsl::to_lower); }

   uint operator&(this const auto&& obj, const auto& other); //strspn
   uint operator^(this const auto&& obj, const auto& other); //strcspn
   uint operator/(this const auto&& obj, const auto& other); //largest n : other*n ⊇ self

   template<typename Self_t> requires str_t<Self_t,char_t> dyn_arr<Self_t> tokenize(this const Self_t&& obj, const str_base& delimChars);
   template<typename Self_t> requires str_t<Self_t,char_t> dyn_arr<Self_t> tokenize(this const Self_t&& obj, const container_base<str_base>& delimStrs);

   //comparison
   template<typename strT> requires str_t<strT,char_t> inline constexpr bool operator==(this auto&& s, const strT& other) { if (s.size() != other.size()) { return false; } return !(s <=> other); }
   template<typename strT> requires str_t<strT,char_t> inline constexpr bool operator!=(this auto&& s, const strT& other) { if (s.size() != other.size()) { return true;  } return   s <=> other;  }

   template<typename strT> requires str_t<strT,char_t> inline constexpr bool operator<(this auto&& s, const strT& other) { return (s <=> other) < 0; }
   template<typename strT> requires str_t<strT,char_t> inline constexpr bool operator<=(this auto&& s, const strT& other) { return (s <=> other) <= 0; }

   template<typename strT> requires str_t<strT,char_t> inline constexpr bool operator>(this auto&& s, const strT& other) { return (s <=> other) > 0; }
   template<typename strT> requires str_t<strT,char_t> inline constexpr bool operator>=(this auto&& s, const strT& other) { return (s <=> other) >= 0; }

   template<typename strT> requires str_t<strT,char_t> inline constexpr sint operator<=>(this auto&& s, const strT& other) { return s.strcmp(other); }



   template<uint len> inline constexpr bool operator==(this auto&& s, const char_t other[len]) { return s == raw_str<len>(other); }
   template<uint len> inline constexpr bool operator!=(this auto&& s, const char_t other[len]) { return s != raw_str<len>(other); }

   template<uint len> inline constexpr bool operator<(this auto&& s, const char_t other[len]) { return s < raw_str<len>(other); }
   template<uint len> inline constexpr bool operator<=(this auto&& s, const char_t other[len]) { return s <= raw_str<len>(other); }

   template<uint len> inline constexpr bool operator>(this auto&& s, const char_t other[len]) { return s > raw_str<len>(other); }
   template<uint len> inline constexpr bool operator>=(this auto&& s, const char_t other[len]) { return s >= raw_str<len>(other); }

   template<uint len> inline constexpr sint operator<=>(this auto&& s, const char_t other[len]) { return s <=> raw_str<len>(other); }


   template<typename strT> requires str_t<strT,char_t> constexpr sint strcmp(this auto&& s, const strT& other) {
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
   template<typename strT> requires str_t<strT,char_t> constexpr sint substrcmp(this auto&& s, const strT& other) {
      uint len = s.size() < other.size() ? s.size() : other.size();
      for (uint i = 0; i < len; ++i) {
         if (s[i] != other[i]) {
            return s[i] - other[i];
         }
      }
      return 0;
   }

   //typecasts
   template<typename strT> requires str_t<strT,char_t> inline constexpr operator bool(this strT&& obj) { return obj.size(); }
   template<typename strT> requires str_t<strT,char_t> inline constexpr operator const char_t*(this const strT&& obj) { return obj.data(); }
   template<typename strT> requires str_t<strT,char_t> inline constexpr operator char_t*(this strT&& obj) { return obj.data(); }
};

#endif //MCSL_STR_BASE_HPP