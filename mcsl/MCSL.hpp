#pragma once
#ifndef MCSL_HPP
#define MCSL_HPP

#include "MCSL_DEFINES.hpp"
#include "mcsl_throw.hpp"
#include "concepts.hpp"

namespace mcsl {
   template<typename first_t, typename second_t = first_t> struct pair;

   template<typename T> struct container_base;

   template<typename T> struct contig_base;
   template<typename T, uint _capacity> class buf;
   template<typename T, uint _capacity> class heap_buf;
   template<typename T, uint _size> class static_arr;
   template<typename T> class array;
   template<typename T> class dyn_arr;
   template<typename T> class arr_span;
   template<typename T> class dyn_arr_span;

   template<typename T> struct bin_node;
   template<typename T, uint_t index_t> struct indexed_node;
   // template<typename T, cmpfunc(T)> class prio_queue;
   template<typename T, typename cmp> requires cmp_t<T,cmp> class prio_queue;

   template<typename char_t> struct str_base;
   class string;
   class dyn_str_span;
   template<uint _size> class raw_str;
   class raw_str_span;
   template<uint _size, typename size_t = byte> class raw_buf_str;

   struct str_hash_func;
   template<typename T> struct hash_func;
   template<container_t T> struct container_hash_func;
   template<contig_container_t T> struct contig_hash_func;

   template<typename T, typename eq, typename hash, uint _bucketSize = DEFAULT_BUCKET_SIZE> class set;       //set using extendible hashing
   template<typename T, typename eq, typename hash, uint _bucketSize = DEFAULT_BUCKET_SIZE> class dyn_set;   //set using dynamic perfect hashing
   template<typename T, uint _size, typename hash, uint _bucketSize = DEFAULT_BUCKET_SIZE> class static_set; //set using FKS perfect hashing
   template<typename key_t, typename val_t, typename eq, typename hash, uint _bucketSize = DEFAULT_BUCKET_SIZE> class map;        //map using extendible hashing
   template<typename key_t, typename val_t, typename eq, typename hash, uint _bucketSize = DEFAULT_BUCKET_SIZE> class dyn_map;    //map using dynamic perfect hashing
   template<typename key_t, typename val_t, uint _size, typename hash, uint _bucketSize = DEFAULT_BUCKET_SIZE> class static_map;  //map using FKS perfect hashing

   template<typename T> class bin_tree;
   template<typename T, uint _size> class static_bin_tree;
   template<typename T, uint _size> class inplace_bin_tree;
   template<typename T, class cmp> requires cmp_t<T,cmp> class bst;
   template<typename T, uint _size, typename cmp> requires cmp_t<T,cmp> class static_bst;
   template<typename T, uint _size, typename cmp> requires cmp_t<T,cmp> class inplace_bst;

   template<typename T, uint _size> class pool;
   template<uint _size> class raw_pool;
   template<uint _size> class mem_stack;
}



// //for nameof() functins
// #include "raw_str.hpp"

#endif //MCSL_HPP