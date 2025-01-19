#pragma once
#ifndef STMT_SEQ_HPP
#define STMT_SEQ_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

struct clef::StatementSequence : public mcsl::contig_base<Stmt*> {
   private:
      mcsl::dyn_arr<Stmt*>* _stmts;
   public:
      static constexpr NodeType nodeType() { return NodeType::STMT_SEQ; }

      StatementSequence(mcsl::dyn_arr<Stmt*>& stmts):_stmts{&stmts} {}


      #pragma region dyn_arr
      auto size() const { return _stmts->size(); }
      auto capacity() const { return _stmts->capacity(); }
      
      auto* data() { return _stmts->data(); }
      auto* begin() { return _stmts->begin(); }
      auto* const* ptr_to_buf() { return _stmts->ptr_to_buf(); }

      const auto* data() const { return _stmts->data(); }
      const auto* begin() const { return _stmts->begin(); }
      const auto* const* ptr_to_buf() const { return _stmts->ptr_to_buf(); }


      bool resize(const uint newSize) { return _stmts->resize(newSize); }
      bool resize_exact(const uint newSize) { return _stmts->resize_exact(newSize); }
      auto* release() { return _stmts->release(); }
      bool push_back(Stmt* obj) { return _stmts->push_back(obj); }
      auto pop_back() { return _stmts->pop_back(); }
      auto* emplace(const uint i, auto&&... args) { return _stmts->emplace(i, std::forward<decltype(args)>(args)...); }
      auto* emplace_back(auto&&... args) { return _stmts->emplace_back(std::forward<decltype(args)>(args)...); }
      #pragma endregion dyn_arr
};

#endif //STMT_SEQ_HPP