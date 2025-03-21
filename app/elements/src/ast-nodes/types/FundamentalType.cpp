#ifndef FUND_TYPE_CPP
#define FUND_TYPE_CPP

#include "ast-nodes/types/FundamentalType.hpp"
#include "pretty-print.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::FundType& obj) {
   file.printf(FMT("fundamental type: %s"), clef::toString(obj.id()));
   return file;
}

#endif //FUND_TYPE_CPP