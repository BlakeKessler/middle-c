#pragma once
#ifndef MATCH_CASES_HPP
#define MATCH_CASES_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::MatchCases : public mcsl::contig_base<mcsl::pair<index<Expr>, index<Scope>>> {
   private:
      mcsl::dyn_arr<mcsl::pair<index<Expr>,index<Scope>>>* _cases; //case expression, scope to jump to
   public:
      static constexpr NodeType nodeType() { return NodeType::MATCH_CASES; }

      MatchCases(mcsl::dyn_arr<mcsl::pair<index<Expr>,index<Scope>>>* cases):_cases{cases} {}


      #pragma region dyn_arr
      auto size() const { return _cases->size(); }
      auto capacity() const { return _cases->capacity(); }

      auto* data() { return _cases->data(); }
      auto* begin() { return _cases->begin(); }
      auto* const* ptr_to_buf() { return _cases->ptr_to_buf(); }

      const auto* data() const { return _cases->data(); }
      const auto* begin() const { return _cases->begin(); }
      const auto* const* ptr_to_buf() const { return _cases->ptr_to_buf(); }


      bool reserve(const uint newSize) { return _cases->reserve(newSize); }
      bool reserve_exact(const uint newSize) { return _cases->reserve_exact(newSize); }
      auto* release() { return _cases->release(); }
      bool push_back(mcsl::pair<index<Expr>,index<Scope>>&& obj) { return _cases->push_back(std::forward<decltype(obj)>(obj)); }
      bool push_back(const mcsl::pair<index<Expr>,index<Scope>>& obj) { return _cases->push_back(obj); }
      auto pop_back() { return _cases->pop_back(); }
      auto* emplace(const uint i, auto&&... args) requires mcsl::valid_ctor<mcsl::pair<index<Expr>, index<Scope>>, decltype(args)...> { return _cases->emplace(i, std::forward<decltype(args)>(args)...); }
      auto* emplace_back(auto&&... args) requires mcsl::valid_ctor<mcsl::pair<index<Expr>, index<Scope>>, decltype(args)...> { return _cases->emplace_back(std::forward<decltype(args)>(args)...); }
      #pragma endregion dyn_arr

      void printf() const;
};

#endif //MATCH_CASES_HPP