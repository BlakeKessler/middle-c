#pragma once
#ifndef AST_IT_HPP
#define AST_IT_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"
#include "Node.hpp"
#include "Token.hpp"

#include "string_like.hpp"

class clef::astIt {
   private:
      SyntaxTree const* _tree;
      mutable NodeID_t _index;
   public:
      #pragma region constructors
      astIt();
      astIt(SyntaxTree const* tree, const NodeID_t index);

      // astIt& operator=(const NodeID_t i) const;
      // astIt& operator=(const uint i) const;
      #pragma endregion constructors

      #pragma region operators
      Node& operator*();
      const Node& operator*() const;
      Node* operator->();
      const Node* operator->() const;
      
      astIt& operator++();
      astIt& operator--();
      astIt& operator>>=(const uint childIndex);
      astIt& toParent();
      const astIt& operator++() const;
      const astIt& operator--() const;
      const astIt& operator>>=(const uint childIndex) const;
      const astIt& toParent() const;

      astIt& incIndex() { ++_index; return self; }
      const astIt& incIndex() const { ++_index; return self; }

      template<uint _size> bool operator==(const mcs::raw_buf_str<_size,byte>& str) const;
      template<uint _size> bool operator==(const mcs::raw_str<_size>& str) const;
      #pragma endregion operators


      NodeID_t index() const { return _index; }
      bool goTo(const NodeID_t i) const;

      #pragma region treeGet
      const Token& token() const;
      astIt next() const;
      astIt prev() const;
      astIt parent() const;
      astIt operator[](const uint i) const;
      #pragma endregion treeGet
      #pragma region treeSet
      astIt& pop();
      astIt& setPrev(const NodeID_t other);
      astIt& setNext(const NodeID_t other);
      astIt& setChild(const NodeID_t other, const byte i);
      astIt& severPrev();
      astIt& severNext();
      inline astIt& sever() { severPrev(); return severNext(); }

      astIt& swap(astIt& other);
      astIt& propegate();
      #pragma endregion treeSet

      void printf() const;

      operator NodeID_t() const { return _index; }
      operator bool() const { return +_index; }
      bool operator!() const { return !(bool)self; } 
      operator Node*() const { return &const_cast<SyntaxTree*>(_tree)->_nodes[+_index]; }
      

   friend class SyntaxTree;
};

template<uint _size> bool clef::astIt::operator==(const mcs::raw_str<_size>& str) const {
   return token() == str;
}
template<uint _size> bool clef::astIt::operator==(const mcs::raw_buf_str<_size,byte>& str) const {
   return token() == str;
}

#endif //AST_IT_HPP