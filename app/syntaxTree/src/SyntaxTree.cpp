#ifndef SYNTAX_TREE_CPP
#define SYNTAX_TREE_CPP

#include "SyntaxTree.hpp"
#include "astIt.hpp"
#include <typeinfo>

//!initialize syntax tree from tokens
clef::SyntaxTree::SyntaxTree(mcsl::dyn_arr<Token>&& tokens, mcsl::array<mcsl::dyn_arr_span<Token>>& lines):
   _root(static_cast<NodeID_t>(1)),
   _nodes(tokens.size()),
   _tokens(std::move(tokens)),
   _lineBounds(lines.size()) {
      //initialize nodes
      _nodes.emplace(0,TOK_NIL);
      for (uint i = 1; i < _tokens.size(); ++i) {
         _nodes.emplace(i,static_cast<NodeID_t>(i+1),static_cast<NodeID_t>(i-1),_tokens[i].nodeType(),static_cast<TokenID_t>(i));
      } _nodes.back().nextID = NODE_NIL;

      //calculate line bounds
      uint temp = 0;
      for (uint i = 1; i < lines.size(); ++i) {
         _lineBounds[i].first = temp;
         temp += lines[i].size();
         _lineBounds[i].second = temp;
      }
}

//!print AST
void clef::SyntaxTree::printf() const {
   //allocate memory
   mcsl::dyn_arr<mcsl::pair<NodeID_t, uint>> stack;
   stack.push_back({_root,0});
   astIt current;
   uint indents;
   uint i;
   //iterate
   while (stack.size()) {
      //current
      indents = stack.back().second;
      current = it(stack.pop_back().first);
      for (i = indents; i; --i) {
         std::printf("   ");
      }
      std::printf("%3u: ", +current->type);
      current.printf();
      std::printf("\n");
      //next
      if (+current->nextID) {
         stack.push_back({current->nextID,indents});
      }
      //children
      ++indents;
      for (i = current->childIDs.size(); i;) {
         if (+current->childIDs[--i]) {
            stack.push_back({current->childIDs[i],indents});
         }
      }
   }
}

//!update the root
//!parents, then predecessors
bool clef::SyntaxTree::updateRoot() {
   astIt root = this->root();
   while (+root->parentID || +root->prevID) {
      while (+root->parentID) {
         root.toParent();
      }
      if (+root->prevID) {
         --root;
      }
   }
   if (root.index() == this->root().index()) {
      return false;
   }
   _root = root.index();
   return true;
}

#pragma region DEBUG
void clef::SyntaxTree::debug_printf() const {
   for (uint i = 0; i < _nodes.size(); ++i) {
      std::printf("%u <- %u(%u)[%u][%u, %u, %u] -> %u",
         +_nodes[i].prevID,
         i,
         +_nodes[i].tokenID,
         +_nodes[i].parentID,
         +_nodes[i].childIDs[0],
         +_nodes[i].childIDs[1],
         +_nodes[i].childIDs[2],
         +_nodes[i].nextID
      );
      if (+_nodes[i].tokenID) {
         std::printf("\t\t\t%.*s",_tokens[+_nodes[i].tokenID].size(),_tokens[+_nodes[i].tokenID].begin());
      }
      std::printf("\n");
   }
}
#pragma endregion DEBUG



#pragma region trivial
clef::astIt clef::SyntaxTree::root() { return astIt(this,_root); }
const clef::astIt clef::SyntaxTree::root() const { return astIt(this,_root); }
clef::astIt clef::SyntaxTree::begin() { return astIt(this,static_cast<NodeID_t>(1)); }
const clef::astIt clef::SyntaxTree::begin() const { return astIt(this,static_cast<NodeID_t>(1)); }
clef::astIt clef::SyntaxTree::end() { return astIt(this, static_cast<NodeID_t>(_nodes.size())); }
const clef::astIt clef::SyntaxTree::end() const { return astIt(this, static_cast<NodeID_t>(_nodes.size())); }
clef::astIt clef::SyntaxTree::null() { return astIt(this, NODE_NIL); }
const clef::astIt clef::SyntaxTree::null() const { return astIt(this, NODE_NIL); }
clef::astIt clef::SyntaxTree::it(const NodeID_t i) { return astIt(this,i); }
const clef::astIt clef::SyntaxTree::it(const NodeID_t i) const { return astIt(this,i); }
#pragma endregion trivial

#endif //SYNTAX_TREE_CPP