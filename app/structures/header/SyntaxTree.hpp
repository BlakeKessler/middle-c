#ifndef SYNTAX_TREE_HPP
#define SYNTAX_TREE_HPP

#include "CLEF.hpp"
#include "SymbolTable.hpp"
#include "astNode.hpp"
#include "allocator.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::SyntaxTree {
   private:
      SymbolTable _names;

      mcsl::dyn_arr<astNode> _buf; //owning storage for member nodes
      mcsl::dyn_arr<mcsl::pair<astNode*>> _spine; //most recent scope and node in that scope and the same for its parent scopes

      astNode* _root;
      astNode* _back;

      allocator _alloc;
   public:
      SyntaxTree():_names{},_buf{},_spine{},_root{},_back{},_alloc{} { astNode::_CURR_ALLOC = &_alloc; }
      // SyntaxTree(SyntaxTree&& other);
      // SyntaxTree(const SyntaxTree& other);
      SyntaxTree(const SyntaxTree& other);
      SyntaxTree(SyntaxTree&& other);
      ~SyntaxTree() { if (astNode::_CURR_ALLOC == &_alloc) { astNode::_CURR_ALLOC = nullptr; } }

      astNode* root() { return _root; }
      mcsl::dyn_arr<mcsl::pair<astNode*>>& spine() { return _spine; }
      const astNode* root() const { return _root; }
      const mcsl::dyn_arr<mcsl::pair<astNode*>>& spine() const { return _spine; }

      void printf() const;
      void print() const;


      Statement* pushStatement(StmtSequence* parent, Statement* stmt);

      Statement* pushForLoop(StmtSequence* parent, Statement* init, Statement* condition, Expression* increment);
      Statement* pushForeachLoop(StmtSequence* parent, Expression* decl, Expression* range);
      Statement* pushWhileLoop(StmtSequence* parent, Expression* condition);

      // Statement* pushTemplate();



      
      bool isComplete() const; //could represent a complete file of source code
      bool assertComplete() const; //checks isComplete and throws errors when something invalid is found


      void pushNode(astNode&& node);
      Statement* endStatement(); //returns statement node

      astNode* openBlock();      //returns previous node
      Block* closeBlock();       //returns the block
};

#endif //SYNTAX_TREE_HPP