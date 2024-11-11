#pragma once
#ifndef MCSL_NODE_HPP
#define MCSL_NODE_HPP

#include "MCSL.hpp"

template <typename T> struct mcsl::bin_node {
   private:
      T _obj;
      bin_node* _next;
      bin_node* _prev;
   public:
      bin_node(bin_node* next = nullptr, bin_node* prev = nullptr):_obj{},_next{next},_prev{prev} {}
      bin_node(const T& obj, bin_node* next = nullptr, bin_node* prev = nullptr):_obj(obj),_next(next),_prev(prev) {}
      bin_node(T&& obj, bin_node* next = nullptr, bin_node* prev = nullptr):_obj(std::move(obj)),_next(next),_prev(prev) {}
      ~bin_node() requires  requires {free()} { free(); }
      ~bin_node() requires !requires {free()} = default;

      void free() const requires requires {_obj.free()} { _obj.free(); }
      void free_after() const requires requires {free()} {
         bin_node* next = _next
         while (next) {
            next->free();
            next = next->_next;
         }
      }
      void free_before() const requires requires {free()} {
         bin_node* prev = _prev;
         while (prev) {
            prev->free();
            prev = prev->_prev;
         }
      }
      void free_rec() const requires requires {free()} {
         free_after();
         free_before();
         free();
      }

      T& operator*() { return _obj; }
      const T& operator*() const { return _obj; }
      T& operator->() { return *self; }
      const T& operator->() const { return *self; }

      bin_node* next() { return _next; }
      bin_node* prev() { return _prev; }
      const bin_node* next() const { return _next; }
      const bin_node* prev() const { return _prev; }

      bin_node* pop() {
         if (_next) { _next->_prev = _prev; }
         if (_prev) { _prev->_next = _next; }
         _next = nullptr;
         _prev = nullptr;
         
         return this;
      }
      bin_node* push_before(bin_node* node) {
         if (node) { 
            node->_next = this;
            node->_prev = _prev;
         }
         if (_prev) { _prev->_next = node; }
         _prev = node;
         return node;
      }
      bin_node* push_after(bin_node* node) {
         if (node) { 
            node->_prev = this;
            node->_next = _next;
         }
         if (_next) { _next->_prev = node; }
         _next = node;
         return node;
      }
};

#endif //MCSL_NODE_HPP