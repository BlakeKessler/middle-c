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
      // bool findFunctions();
      bool createStatements();
      bool handleOperators();
      //helper functions for parsing
      uint escape(astIt& it);
      astIt makeBlock(astIt& open, astIt& close);
      astIt makePtxtSeg(astIt& open, astIt& close);
      // astIt makeFunction(astIt& iden, astIt& operands);
      astIt makeStatement(astIt& open, astIt& close);
      astIt makeOpNode(astIt& op);

      inline bool updateRoot() { return _tree.updateRoot(); }
   public:
      //constructors and destructor
      Parser() = default;
      Parser(mcsl::dyn_arr<Token>&& tokens, mcsl::array<mcsl::dyn_arr_span<Token>>& lines);
      Parser(Parser&& other): _tree(std::move(other._tree)) { other.release(); }
      ~Parser() = default;
      void release() { _tree.release(); }

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