#ifndef FUND_TYPE_CPP
#define FUND_TYPE_CPP

#include "ast-nodes/types/FundamentalType.hpp"
#include "pretty-print.hpp"

#include "io.hpp"

void clef::FundType::printf() const {
   mcsl::printf(mcsl::FMT("fundamental type: %s"), toString(id()));
}

#endif //FUND_TYPE_CPP