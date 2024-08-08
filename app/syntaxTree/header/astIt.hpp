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

      template<uint _size> bool operator==(const mcs::raw_str<_size>& str) const;
      #pragma endregion operators


      NodeID_t index() const { return _index; }
      bool setIndex(const NodeID_t i) const;

      #pragma region treeGet
      const Token& token() const;
      astIt next() const;
      astIt prev() const;
      astIt parent() const;
      astIt operator[](const uint i) const;
      #pragma endregion treeGet
      #pragma region treeSet
      astIt& setPrev(const NodeID_t other);
      astIt& setNext(const NodeID_t other);
      astIt& setChild(const NodeID_t other, const byte i);
      astIt& severPrev();
      astIt& severNext();

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
   // const uint len = std::strlen(str.begin());
   // assert(len <= _size);
   // // std::printf("%.*s == ",_size,str.begin()); printf(); std::printf("\n");
   // if (len != token().size()) {
   //    return false;
   // }
   // return !std::strncmp(token().begin(),str.begin(),len);
}

#endif //AST_IT_HPP