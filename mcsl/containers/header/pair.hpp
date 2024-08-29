#pragma once
#ifndef MCSL_PAIR_HPP
#define MCSL_PAIR_HPP

#include "MCSL.hpp"

template<typename first_t, typename second_t> struct mcsl::pair {
   first_t first;
   second_t second;
   
   constexpr pair() {}
   constexpr pair(const first_t a, const second_t b):first(a),second(b) {}
};

// template<typename first_t, typename second_t> mcsl::pair(first_t,second_t) -> mcsl::pair<first_t,second_t>;

#endif //MCSL_PAIR_HPP