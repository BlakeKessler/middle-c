#pragma once
#ifndef MCSL_TYPE_TRAITS_HPP
#define MCSL_TYPE_TRAITS_HPP

#include "MCSL.hpp"
#include "concepts.hpp"

namespace mcsl {
   //max and min values for integer types
   template <uint_t T> constexpr lsint TYPEMIN() { return 0; }
   template <sint_t T> constexpr lsint TYPEMIN() { return (T)(1 << (8 * sizeof(T) - 1)); }
   template < int_t T> constexpr luint TYPEMAX() { return (T)(~TYPEMIN<T>()); }
};

#endif //MCSL_TYPE_TRAITS_HPP