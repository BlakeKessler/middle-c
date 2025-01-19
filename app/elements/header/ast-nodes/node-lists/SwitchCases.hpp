#pragma once
#ifndef SWITCH_CASES_HPP
#define SWITCH_CASES_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"
#include "pair.hpp"

struct clef::SwitchCases : public mcsl::contig_base<mcsl::pair<Expr*,Stmt*>> {
   private:
      Scope* _proc;
      mcsl::dyn_arr<mcsl::pair<Expr*,Stmt*>>* _cases; //case expression, statement to jump to
   public:
      static constexpr NodeType nodeType() { return NodeType::SWITCH_CASES; }

      SwitchCases(mcsl::dyn_arr<mcsl::pair<Expr*,Stmt*>>& cases, Scope* procedure = nullptr):_proc{procedure},_cases{&cases} {}

      Scope*& procedure() { return _proc; }
      auto*& cases() { return _cases; }

      const Scope* procedure() const { return _proc; }
      const auto* cases() const { return _cases; }

      #pragma region dyn_arr
      auto size() const { return _cases->size(); }
      auto capacity() const { return _cases->capacity(); }

      auto* data() { return _cases->data(); }
      auto* begin() { return _cases->begin(); }
      auto* const* ptr_to_buf() { return _cases->ptr_to_buf(); }

      const auto* data() const { return _cases->data(); }
      const auto* begin() const { return _cases->begin(); }
      const auto* const* ptr_to_buf() const { return _cases->ptr_to_buf(); }


      bool resize(const uint newSize) { return _cases->resize(newSize); }
      bool resize_exact(const uint newSize) { return _cases->resize_exact(newSize); }
      auto* release() { return _cases->release(); }
      bool push_back(mcsl::pair<Expr*,Stmt*>&& obj) { return _cases->push_back(std::forward<decltype(obj)>(obj)); }
      bool push_back(const mcsl::pair<Expr*,Stmt*>& obj) { return _cases->push_back(obj); }
      auto pop_back() { return _cases->pop_back(); }
      auto* emplace(const uint i, auto&&... args) { return _cases->emplace(i, std::forward<decltype(args)>(args)...); }
      auto* emplace_back(auto&&... args) { return _cases->emplace_back(std::forward<decltype(args)>(args)...); }
      #pragma endregion dyn_arr
};

#endif //SWITCH_CASES_HPP