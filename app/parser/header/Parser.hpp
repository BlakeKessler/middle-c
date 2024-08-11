#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"
#include "astIt.hpp"
#include "alloc.hpp"

class clef::Parser {
   private:
      SyntaxTree _tree;

      //sections of parsing
      bool escape();
      bool removePtxt();
      bool handleBlockDelims();
      //helper functions for parsing
      uint escape(astIt& it);
      astIt makeBlock(astIt& open, astIt& close);
      astIt makePtxtSeg(astIt& open, astIt& close);
   public:
      //constructors and destructor
      Parser() = default;
      Parser(mcs::dyn_arr<Token>&& tokens, mcs::array<mcs::dyn_arr_span<Token>>& lines);
      ~Parser() = default;

      //parse
      bool runPass();
      bool runPasses(const uint maxPasses);

      //getters
      const SyntaxTree& getTree() const {return _tree;}


      //IO
      void printf() const { _tree.printf(); }
      void print() const { _tree.debug_printf(); }
};

#endif //PARSER_HPP