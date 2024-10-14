#pragma once
#ifndef MCSL_STATIC_BST_HPP
#define MCSL_STATIC_BST_HPP

#include "MCSL.hpp"

template<typename T, uint _size, typename cmp>
class mcsl::static_bst {
   public:
      struct node;
   private:
      T _buf[_size];
      node _pointerBuf[_size];
      node* _root;

   public:
      constexpr static_bst();
      template<container_t other_t> constexpr static_bst(const other_t& other);
      constexpr static_bst(const auto&&... initList);

      T* insert(const T& obj);
      void erase(T* obj);

      template<typename obj_t> bool contains(const obj_t& obj) const
         requires (const T&& elem, const obj_t&& obj) { {cmp(elem, obj)} -> is<bool>; };
      template<typename obj_t> bool find(const obj_t& obj) const
         requires (const T&& elem, const obj_t&& obj) { {cmp(elem, obj)} -> is<bool>; };
};

template<typename T, uint _size, typename cmp>
struct mcsl::static_bst<T,_size,cmp>::node {
   uint _self;
   uint _lhs;
   uint _rhs;
   uint _parent;
};

#endif //MCSL_STATIC_BST_HPP