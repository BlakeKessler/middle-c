#pragma once
#ifndef MCS_HPP
#define MCS_HPP

#include "MCS_DEFINES.hpp"
#include "mcs_throw.hpp"

namespace mcs {
   template<typename T, uint _size> class static_arr;
   template<typename T> class array;
   template<typename T> class dyn_arr;
   template<typename T> class arr_span;
   template<typename T> class dyn_arr_span;

   // template<typename T> class node;
   // template<typename T, cmpfunc(T)> class prio_queue;

   class string;
   class dyn_str_span;
   template<uint _size> class raw_str;
   class raw_str_span;

   template<typename first_t, typename second_t = first_t> struct pair;
}

#endif //MCS_HPP