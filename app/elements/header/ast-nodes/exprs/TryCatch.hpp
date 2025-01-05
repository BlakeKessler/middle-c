#pragma once
#ifndef TRY_CATCH_HPP
#define TRY_CATCH_HPP

#include "CLEF.hpp"

struct clef::TryCatch : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::TRY_CATCH; }
      TryCatch():Statement{} {}
      TryCatch(Scope* procedure, Decl* err, Scope* errHandler):
         Statement{OpID::TRY_CATCH,NodeType::SCOPE,NodeType::DECL,NodeType::SCOPE,procedure,err,errHandler} {}

      Scope*& procedure() { return (Scope*&)_lhs; }
      Decl*& err() { return (Decl*&)_rhs; }
      Scope*& errHandler() { return (Scope*&)_extra; }
      const Scope* procedure() const { return (Scope*)_lhs; }
      const Decl* err() const { return (Decl*)_rhs; }
      const Scope* errHandler() const { return (Scope*)_extra; }
};

#endif //TRY_CATCH_HPP