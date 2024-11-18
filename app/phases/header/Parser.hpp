#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SourceTokens.hpp"
#include "SyntaxTree.hpp"

#include "MidC_Data.hpp"
#include "OperatorData.hpp"

#include "alloc.hpp"

class clef::Parser {
   private:
      static constexpr const mcsl::raw_str _nameof = "Parser";
   public:
      static constexpr const auto& nameof() { return _nameof; }

      //parse tokenized source code
      static SyntaxTree parse(const SourceTokens& src);
};

#endif //PARSER_HPP