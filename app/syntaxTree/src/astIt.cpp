#ifndef AST_IT_CPP
#define AST_IT_CPP

#include "astIt.hpp"

#pragma region constructors
clef::astIt::astIt():_tree(nullptr),_index() {}
clef::astIt::astIt(SyntaxTree const* tree, const NodeID_t index)
   :_tree(tree),_index(index) {

}

// clef::astIt& clef::astIt::operator=(const NodeID_t i) const {
//    _index = i;
//    return *const_cast<astIt*>(this);
// }
// clef::astIt& clef::astIt::operator=(const uint i) const {
//    _index = static_cast<NodeID_t>(i);
//    return *const_cast<astIt*>(this);
// }
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
   return *this;
}
const clef::astIt& clef::astIt::operator++() const {
   _index = _tree->_nodes[+_index].nextID;
   return *this;
}
clef::astIt& clef::astIt::operator--() {
   _index = _tree->_nodes[+_index].prevID;
   return *this;
}
const clef::astIt& clef::astIt::operator--() const {
   _index = _tree->_nodes[+_index].prevID;
   return *this;
}
clef::astIt& clef::astIt::operator>>=(const uint childIndex) {
   _index = _tree->_nodes[+_index].childIDs[childIndex];
   return *this;
}
const clef::astIt& clef::astIt::operator>>=(const uint childIndex) const {
   _index = _tree->_nodes[+_index].childIDs[childIndex];
   return *this;
}
clef::astIt& clef::astIt::toParent() {
   _index = _tree->_nodes[+_index].parentID;
   return *this;
}
const clef::astIt& clef::astIt::toParent() const {
   _index = _tree->_nodes[+_index].parentID;
   return *this;
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
   return *this;
}
//!set next and adjust tree to maintain consistency
clef::astIt& clef::astIt::setNext(const NodeID_t other) {
   // assert(_index);
   if (!_index) { return *this; }
   if (+other) {
      const_cast<SyntaxTree*>(_tree)->_nodes[+other].prevID = _index;
   }
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].nextID = other;
   return *this;
}
//!set ith child and adjust tree to maintain consistency
clef::astIt& clef::astIt::setChild(const NodeID_t other, const byte i) {
   // assert(_index);
   if (!_index) { return *this; }
   if (+other) {
      const_cast<SyntaxTree*>(_tree)->_nodes[+other].parentID = _index;
   }
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].childIDs[i] = other;
   return *this;
}

//!break link between this and prev
clef::astIt& clef::astIt::severPrev() {
   const_cast<SyntaxTree*>(_tree)->_nodes[+_tree->_nodes[+_index].prevID].nextID = NODE_NIL;
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].prevID = NODE_NIL;
   return *this;
}
//!break link between this and next
clef::astIt& clef::astIt::severNext() {
   const_cast<SyntaxTree*>(_tree)->_nodes[+_tree->_nodes[+_index].nextID].prevID = NODE_NIL;
   const_cast<SyntaxTree*>(_tree)->_nodes[+_index].nextID = NODE_NIL;
   return *this;
}
#pragma endregion treeSet

void clef::astIt::printf() const {
   _tree->_tokens[+_tree->_nodes[+_index].tokenID].printf();
}

#endif //AST_IT_CPP