#pragma once
#ifndef MCSL_CONCEPTS
#define MCSL_CONCEPTS

#include "MCSL.hpp"

#include <concepts>


//!NOTE: FIX CONTAINER CONCEPTS


namespace mcsl {
   #pragma region basic
   template<typename lhs, typename rhs> concept is_t = std::derived_from<lhs,rhs>;
   // template<typename lhs, typename rhs> concept priv_is_t = requires {
   //    std::is_base_of_v<rhs,lhs>;
   // };
   template<typename lhs, typename rhs> concept same_t = std::same_as<lhs,rhs>;
   template<typename T> concept int_t = std::integral<T>;
   template<typename T> concept uint_t = std::unsigned_integral<T>;
   template<typename T> concept sint_t = std::signed_integral<T>;
   template<typename T> concept float_t = std::floating_point<T>;
   template<typename T> concept num_t = mcsl::int_t<T> || mcsl::float_t<T>;

   template<typename T> concept ptr_t = requires (T a) {
      std::is_pointer_v<T>;
   };
   #pragma endregion basic

   #pragma region containerss
   template<typename T> concept contig_container_t = requires(T a) {
      { a.begin() };
      { a.size() } -> std::integral;
      std::is_pointer(a.begin());
   };
   template<typename T> concept container_t = requires(T a) {
      { a.begin() };
      { a.end() };
      std::is_pointer(a.begin());
      std::is_same(a.begin(), a.end());
   };

   template<typename arr_t> concept contig_t = requires (arr_t arr) {
      { arr.data()[0] } -> std::same_as<decltype(arr[0])>;
      { arr.begin() } -> std::same_as<decltype(arr.data())>;
      { arr.end() } -> std::same_as<decltype(arr.data())>;
      { arr.size() } -> std::integral;
   } && arr_t::is_contig;
   template<typename string_t> concept str_t = requires (string_t str) {
      { str[0] } -> std::convertible_to<char>;
   } && contig_t<string_t>;

   template<typename set_t, typename T> concept assoc_t = requires ( set_t set, T obj) {
      { set_t::is_assoc } -> std::same_as<void>;
      { set.contains(obj) } -> std::same_as<bool>;
   };
   #pragma endregion containers

   #pragma region utils
   template<typename T, typename func> concept hash_t = requires (T obj, func f) {
      { f(obj) } -> std::integral;
   };
   template<typename T, typename func> concept cmp_t = requires (T lhs, T rhs, func f) {
      { f(lhs,rhs) } -> std::convertible_to<bool>;
   };

   template<typename func_t, typename ...Args> concept callable_t = requires (func_t f, Args... args) {
      f(args...);
   };
   #pragma endregion utils
};

#endif //MCSL_CONCEPTS