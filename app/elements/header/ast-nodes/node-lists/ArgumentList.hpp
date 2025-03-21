#pragma once
#ifndef ARG_LIST_HPP
#define ARG_LIST_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

struct clef::ArgumentList : public mcsl::contig_base<index<Expr>> {
   private:
      mcsl::dyn_arr<index<Expr>>* _exprs;
   public:
      static constexpr NodeType nodeType() { return NodeType::ARG_LIST; }

      ArgumentList(mcsl::dyn_arr<index<Expr>>* exprs):_exprs{exprs} {}


      #pragma region dyn_arr
      auto size() const { return _exprs->size(); }
      auto capacity() const { return _exprs->capacity(); }

      auto* data() { return _exprs->data(); }
      auto* begin() { return _exprs->begin(); }
      auto* const* ptr_to_buf() { return _exprs->ptr_to_buf(); }

      const auto* data() const { return _exprs->data(); }
      const auto* begin() const { return _exprs->begin(); }
      const auto* const* ptr_to_buf() const { return _exprs->ptr_to_buf(); }


      bool reserve(const uint newSize) { return _exprs->reserve(newSize); }
      bool reserve_exact(const uint newSize) { return _exprs->reserve_exact(newSize); }
      auto* release() { return _exprs->release(); }
      bool push_back(index<Expr> obj) { return _exprs->push_back(obj); }
      auto pop_back() { return _exprs->pop_back(); }
      auto* emplace(const uint i, auto&&... args) requires mcsl::valid_ctor<Expr, decltype(args)...> { return _exprs->emplace(i, std::forward<decltype(args)>(args)...); }
      auto* emplace_back(auto&&... args) requires mcsl::valid_ctor<Expr, decltype(args)...> { return _exprs->emplace_back(std::forward<decltype(args)>(args)...); }
      #pragma endregion dyn_arr
};

namespace mcsl {
   File& write(File&, const clef::ArgList&);
};

#endif //ARG_LIST_HPP