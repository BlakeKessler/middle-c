#pragma once
#ifndef MCSL_PRIO_QUEUE_HPP
#define MCSL_PRIO_QUEUE_HPP

#include "MCSL.hpp"

template<typename T, typename cmp_t> class mcsl::prio_queue {
   public:
      class node;
   private:
      node* _root;
      uint _size;

   public:
      constexpr prio_queue(): _root(), _size(0) {}
      prio_queue(prio_queue&& other);
      template<is_t<T> args_t...> prio_queue(args_t&& args);
      template<container_t<T> other_t> prio_queue(other_t&& other);

      void push(T&& obj);
      template<is_t<T> args_t...> void push(args_t... objs);
      void push(node* node);
      void push(node* node, const uint depth);
      template<container_t<T> arr_t> void push_contents(arr_t&& objs);

      void emplace(auto&&... args);

      void pop();
      const T& top();
};

#endif //MCSL_PRIO_QUEUE_HPP