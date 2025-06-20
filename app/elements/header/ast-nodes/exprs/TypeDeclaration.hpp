#pragma once
#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Statement.hpp"

struct clef::TypeDeclaration : public clef::Statement {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::MAKE_TYPE; }
      static constexpr OpID pseudoOpID() { return OpID::MAKE_TYPE; }

      TypeDeclaration():Statement{} {}
      TypeDeclaration(index<Identifier> name):Statement{OpID::MAKE_TYPE,name} {}


      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_lhs); }
      index<const Identifier> name() const { return _lhs; }
};

namespace mcsl {
   File& write(File&, const clef::Decl&);
};

#endif //TYPE_DECL_HPP