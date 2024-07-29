#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"
#include "Node.hpp"
#include "Tokenizer.hpp"

class clef::Parser {
   private:
      SyntaxTree* _tree;

      //private helper functions
      bool runPass(Node* current);
      bool evalDelimPair(Node* current);

      bool handleDelimPairs();
   public:
      //constructors and destructor
      Parser() { _tree = nullptr; }
      Parser(SyntaxTree* tree) { _tree = tree; }
      Parser(Token* tokens, const uint count) { _tree = new SyntaxTree(tokens, count); }
      Parser(Tokenizer* tokens) {_tree = new SyntaxTree(tokens->begin(), tokens->size());}
      ~Parser() {delete _tree;}

      //parse
      bool runPass();
      bool runPasses(const uint maxPasses);

      //getters
      const SyntaxTree* getTree() const {return _tree;}
};

#endif //PARSER_HPP