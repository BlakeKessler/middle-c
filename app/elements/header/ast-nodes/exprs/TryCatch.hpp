#pragma once
#ifndef TRY_CATCH_HPP
#define TRY_CATCH_HPP

#include "CLEF.hpp"

#include "ast-nodes/Statement.hpp"

struct clef::TryCatch : public clef::Statement {
   private:

   public:
      static constexpr NodeType nodeType() { return NodeType::TRY_CATCH; }
      static constexpr OpID pseudoOpID() { return OpID::TRY_CATCH; }
      TryCatch():Statement{} {}
      TryCatch(index<Scope> procedure, index<Decl> err, index<Scope> errHandler):
         Statement{OpID::TRY_CATCH,NodeType::SCOPE,NodeType::DECL,NodeType::SCOPE,procedure,err,errHandler} {}

      index<Scope>& procedure() { return reinterpret_cast<index<Scope>&>(_lhs); }
      index<Decl>& err() { return reinterpret_cast<index<Decl>&>(_rhs); }
      index<Scope>& errHandler() { return reinterpret_cast<index<Scope>&>(_extra); }
      index<const Scope> procedure() const { return _lhs; }
      index<const Decl> err() const { return _rhs; }
      index<const Scope> errHandler() const { return _extra; }
};

namespace mcsl {
   File& write(File&, const clef::TryCatch&);
};

#endif //TRY_CATCH_HPP