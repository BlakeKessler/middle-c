#pragma once
#ifndef PAIR_HPP
#define PAIR_HPP

#include "MCS.hpp"

template<typename first_t, typename second_t> struct mcs::pair {
   first_t first;
   second_t second;
   
   pair() {}
   pair(const first_t a, const second_t b):first(a),second(b) {}
};

#endif //PAIR_HPP