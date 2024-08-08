#ifndef AST_IT_CPP
#define AST_IT_CPP

#include "astIt.hpp"

#pragma region constructors
clef::astIt::astIt():_tree(nullptr),_index() {}
clef::astIt::astIt(SyntaxTree const* tree, const NodeID_t index)
   :_tree(tree),_index(index) {

}
#pragma endregion constructors

#pragma region operators
clef::Node& clef::astIt::operator*() {
   return const_cast<SyntaxTree*>(_tree)->_nodes[+_index];
}
const clef::Node& clef::astIt::operator*() const {
   return _tree->_nodes[+_index];
}
clef::Node* clef::astIt::operator->() {
   return const_cast<SyntaxTree*>(_tree)->_nodes.begin() + +_index;
}
const clef::Node* clef::astIt::operator->() const {
   return _tree->_nodes.begin() + +_index;
}

clef::astIt& clef::astIt::operator++() {
   _index = _tree->_nodes[+_index].nextID;
   return self;
}
const clef::astIt& clef::astIt::operator++() const {
   _index = _tree->_nodes[+_index].nextID;
   return self;
}
clef::astIt& clef::astIt::operator--() {
   _index = _tree->_nodes[+_index].prevID;
   return self;
}
const clef::astIt& clef::astIt::operator--() const {
   _index = _tree->_nodes[+_index].prevID;
   return self;
}
clef::astIt& clef::astIt::operator>>=(const uint childIndex) {
   _index = _tree->_nodes[+_index].childIDs[childIndex];
   return self;
}
const clef::astIt& clef::astIt::operator>>=(const uint childIndex) const {
   _index = _tree->_nodes[+_index].childIDs[childIndex];
   return self;
}
clef::astIt& clef::astIt::toParent() {
   _index = _tree->_nodes[+_index].parentID;
   return self;
}
const clef::astIt& clef::astIt::toParent() const {
   _index = _tree->_nodes[+_index].parentID;
   return self;
}

#pragma endregion operators

bool clef::astIt::setIndex(const NodeID_t i) const {
   assert(+i < _tree->_nodes.size());
   _index = i;
   return true;
}

#pragma region treeGet
const clef::Token& clef::astIt::token() const {
   return _tree->_tokens[+_tree->_nodes[+_index].tokenID];
}
clef::astIt clef::astIt::next() const {
   return astIt(_tree,_tree->_nodes[+_index].nextID);
}
clef::astIt clef::astIt::prev() const {
   return astIt(_tree,_tree->_nodes[+_index].prevID);
}
clef::astIt clef::astIt::parent() const {
   return astIt(_tree,_tree->_nodes[+_index].parentID);
}
clef::astIt clef::astIt::operator[](const uint i) const {
   return astIt(_tree,_tree->_nodes[+_index].childIDs[i]);
}
#pragma endregion treeGet
#pragma region treeSet
//!set previous and adjust tree to maintain consistency
clef::astIt& clef::astIt::setPrev(const NodeID_t other) {
   assert(_index);
   if (+other) {
      const_cast<SyntaxTree*>(_tree)->_nodes[+other].nextID = _index;
   }
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].prevID = other;
   return self;
}
//!set next and adjust tree to maintain consistency
clef::astIt& clef::astIt::setNext(const NodeID_t other) {
   assert(_index);
   // if (!_index) { return self; }
   if (+other) {
      const_cast<SyntaxTree*>(_tree)->_nodes[+other].prevID = _index;
   }
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].nextID = other;
   return self;
}
//!set ith child and adjust tree to maintain consistency
clef::astIt& clef::astIt::setChild(const NodeID_t other, const byte i) {
   // assert(_index);
   if (!_index) { return self; }
   if (+other) {
      const_cast<SyntaxTree*>(_tree)->_nodes[+other].parentID = _index;
      const_cast<SyntaxTree*>(_tree)->_nodes[+other].indexInParent = i;
   }
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].childIDs[i] = other;
   return self;
}

//!break link between this and prev
clef::astIt& clef::astIt::severPrev() {
   const_cast<SyntaxTree*>(_tree)->_nodes[+_tree->_nodes[+_index].prevID].nextID = NODE_NIL;
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].prevID = NODE_NIL;
   return self;
}
//!break link between this and next
clef::astIt& clef::astIt::severNext() {
   const_cast<SyntaxTree*>(_tree)->_nodes[+_tree->_nodes[+_index].nextID].prevID = NODE_NIL;
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].nextID = NODE_NIL;
   return self;
}

//!adjust tree to be consistent with this node
clef::astIt& clef::astIt::propegate() {
   if (!+_index) { return self; }
   Node* node = &const_cast<SyntaxTree*>(_tree)->_nodes[+_index];

   if (+node->nextID) { next()->prevID = _index; }
   if (+node->prevID) { prev()->nextID = _index; }
   if (+node->parentID) { parent()->childIDs[+node->indexInParent] = _index; }

   for (uint i = 0; i < MAX_AST_CHILDREN; ++i) {
      if (+node->childIDs[i]) {
         self[i]->parentID = _index;
         self[i]->indexInParent = i;
      }
   }

   return self;
}
#pragma endregion treeSet

void clef::astIt::printf() const {
   _tree->_tokens[+_tree->_nodes[+_index].tokenID].printf();
}

#endif //AST_IT_CPP