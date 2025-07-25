#pragma once
#ifndef STMT_SEQ_HPP
#define STMT_SEQ_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

struct clef::StmtSeq : public mcsl::contig_base<index<Stmt>> {
   private:
      mcsl::dyn_arr<index<Stmt>>* _stmts;
   public:
      static constexpr NodeType nodeType() { return NodeType::STMT_SEQ; }

      StmtSeq(mcsl::dyn_arr<index<Stmt>>* stmts):_stmts{stmts} {}


      #pragma region dyn_arr
      auto size() const { return _stmts->size(); }
      auto capacity() const { return _stmts->capacity(); }
      
      auto* data() { return _stmts->data(); }
      auto* begin() { return _stmts->begin(); }
      auto* const* ptr_to_buf() { return _stmts->ptr_to_buf(); }

      const auto* data() const { return _stmts->data(); }
      const auto* begin() const { return _stmts->begin(); }
      const auto* const* ptr_to_buf() const { return _stmts->ptr_to_buf(); }


      bool reserve(const uint newSize) { return _stmts->reserve(newSize); }
      bool reserve_exact(const uint newSize) { return _stmts->reserve_exact(newSize); }
      auto* release() { return _stmts->release(); }
      bool push_back(index<Stmt> obj) { return _stmts->push_back(obj); }
      auto pop_back() { return _stmts->pop_back(); }
      auto* emplace(const uint i, auto&&... args) requires mcsl::valid_ctor<Stmt, decltype(args)...> { return _stmts->emplace(i, std::forward<decltype(args)>(args)...); }
      auto* emplace_back(auto&&... args) requires mcsl::valid_ctor<Stmt, decltype(args)...> { return _stmts->emplace_back(std::forward<decltype(args)>(args)...); }
      #pragma endregion dyn_arr
};

namespace mcsl {
   File& write(File&, const clef::StmtSeq&);
};

#endif //STMT_SEQ_HPP