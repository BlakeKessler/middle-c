#ifndef FUND_TYPE_CPP
#define FUND_TYPE_CPP

#include "ast-nodes/types/FundamentalType.hpp"
#include "pretty-print.hpp"

#include <cstdio>

void clef::FundType::printf() const {
   std::printf("fundamental type: %s", toString(id()));
}

#endif //FUND_TYPE_CPP