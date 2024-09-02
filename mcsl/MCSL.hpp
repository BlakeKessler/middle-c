#pragma once
#ifndef MCSL_HPP
#define MCSL_HPP

#include "MCSL_DEFINES.hpp"
#include "mcsl_throw.hpp"

#include <concepts>

namespace mcsl {
   template<typename T> concept contig_container_t = requires(T a) {
      a.data();
      a.size();
      std::is_pointer(a.data());
      std::is_integral(a.size());
   };
   template<typename T> concept container_t = requires(T a) {
      a.begin();
      a.end();
      std::is_pointer(a.begin());
      std::is_same(a.begin(), a.end());
   };
   template<typename T> concept hashing_class = requires(T a) {
      T::is_hash;
   };

   template<typename T> struct container_base;
   template<typename T> struct contig_base;
   template<typename T, uint _size> class static_arr;
   template<typename T> class array;
   template<typename T> class dyn_arr;
   template<typename T> class arr_span;
   template<typename T> class dyn_arr_span;

   template<typename T, typename hash_func> class set;
   template<typename key_t, typename val_t, typename hash_func> class map;

   template<typename T> struct hash_func;
   template<container_t T> struct container_hash_func;
   template<contig_container_t T> struct contig_hash_func;

   // template<typename T> class node;
   // template<typename T, cmpfunc(T)> class prio_queue;

   template<typename char_t> struct str_base;
   class string;
   class dyn_str_span;
   template<uint _size> class raw_str;
   class raw_str_span;
   template<uint _size, typename size_t = byte> class raw_buf_str;

   struct str_hash_func;

   template<typename first_t, typename second_t = first_t> struct pair;
}



// //for name() functins
// #include "raw_str.hpp"

#endif //MCSL_HPP