#pragma once
#ifndef OPERATOR_DATA_HPP
#define OPERATOR_DATA_HPP

#include "CLEF.hpp"

#include "OpData.hpp"
#include "OpDecoder.hpp"

namespace clef {
   static constexpr const OpDecoder OPERATORS = GetOpData();
   static constexpr const OpDecoder BLOCK_DELIMS = GetBlockData();
   static constexpr const OpDecoder PTXT_DELIMS = GetPtxtData();

};

#endif //OPERATOR_DATA_HPP