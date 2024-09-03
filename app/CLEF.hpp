#pragma once
#ifndef CLEF_HPP
#define CLEF_HPP

#include "CLEF_DEFINES.hpp"
#include "ENUMS.hpp"
#include "CLEF_ERR.hpp"

#include "raw_str.hpp"

namespace clef {
   class Source;

   class Token;
   class Tokenizer;

   struct Operator;
   struct DelimPair;

   struct Node;
   class SyntaxTree;
   class astIt;

   class Parser;
   
   class ErrorHandler;
}

#endif //CLEF_HPP