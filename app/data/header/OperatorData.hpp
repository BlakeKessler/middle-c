#pragma once
#ifndef OPERATOR_DATA_HPP
#define OPERATOR_DATA_HPP

#include "CLEF.hpp"

#include "OpData.hpp"
#include "OpDecoder.hpp"
#include "OpPrecs.hpp"

namespace clef {
   /*constexpr*/ const OpDecoder OPERATORS = GetAllOplikesData();
   constexpr OpPrecs PRECS = GetAllOpPrecsData();
};

#endif //OPERATOR_DATA_HPP