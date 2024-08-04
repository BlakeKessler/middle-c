#pragma once
#ifndef PAIR_HPP
#define PAIR_HPP

#include "MCS.hpp"

template<typename first_t, typename second_t> struct mcs::pair {
   first_t first;
   second_t second;
   
   constexpr pair() {}
   constexpr pair(const first_t a, const second_t b):first(a),second(b) {}
};

// template<typename first_t, typename second_t> mcs::pair(first_t,second_t) -> mcs::pair<first_t,second_t>;

#endif //PAIR_HPP