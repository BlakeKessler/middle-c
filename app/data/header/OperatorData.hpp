#pragma once
#ifndef OPERATOR_DATA_HPP
#define OPERATOR_DATA_HPP

#include "CLEF.hpp"

#include "OpData.hpp"
#include "OpDecoder.hpp"

namespace clef {
   constexpr const OpDecoder ALL_OPLIKES = GetAllOplikesData();
   constexpr const OpDecoder OPERATORS = GetOpData();
   constexpr const OpDecoder BLOCK_DELIMS = GetBlockData();
   constexpr const OpDecoder PTXT_DELIMS = GetPtxtData();





   constexpr const OpGroup OPEN_PARENS       = BLOCK_DELIMS["("];
   constexpr const OpGroup OPEN_INIT_LIST    = BLOCK_DELIMS["{"];
   constexpr const OpGroup OPEN_SUBSCRIPT    = BLOCK_DELIMS["["];
   
   constexpr const OpGroup CLOSE_PARENS      = BLOCK_DELIMS[")"];
   constexpr const OpGroup CLOSE_INIT_LIST   = BLOCK_DELIMS["}"];
   constexpr const OpGroup CLOSE_SUBSCRIPT   = BLOCK_DELIMS["]"];



   constexpr const OpGroup LINE_CMNT         = PTXT_DELIMS["//"];
   constexpr const OpGroup BLOCK_CMNT_OPEN   = PTXT_DELIMS["/*"];
   constexpr const OpGroup BLOCK_CMNT_CLOSE  = PTXT_DELIMS["*/"];

   constexpr const OpGroup STR_OP            = PTXT_DELIMS["\""];
   constexpr const OpGroup CHAR_OP           = PTXT_DELIMS["\'"];



   constexpr const OpGroup LABEL_DELIM       = OPERATORS[":"];



   constexpr const OpGroup RADIX_POINT       = OPERATORS["."];
};

#endif //OPERATOR_DATA_HPP