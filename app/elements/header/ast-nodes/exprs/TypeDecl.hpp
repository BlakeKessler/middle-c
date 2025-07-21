#pragma once
#ifndef TYPE_DECL_HPP
#define TYPE_DECL_HPP

#include "CLEF.hpp"
#include "ast-nodes/Stmt.hpp"

struct clef::TypeDecl : public clef::Stmt {
   private:
   
   public:
      static constexpr NodeType nodeType() { return NodeType::MAKE_TYPE; }
      static constexpr OpID pseudoOpID() { return OpID::MAKE_TYPE; }

      TypeDecl():Stmt{} {}
      TypeDecl(index<Identifier> name):Stmt{OpID::MAKE_TYPE, name} {} //forward declaration
      TypeDecl(index<Identifier> name, index<Identifier> decl):Stmt{OpID::MAKE_TYPE, name, decl} {}


      index<Identifier>& name() { return reinterpret_cast<index<Identifier>&>(_lhs); }
      index<const Identifier> name() const { return _lhs; }
      index<Identifier>& decl() { return reinterpret_cast<index<Identifier>&>(_rhs); }
      index<const Identifier> decl() const { return _rhs; }
};

namespace mcsl {
   File& write(File&, const clef::Decl&);
};

#endif //TYPE_DECL_HPP