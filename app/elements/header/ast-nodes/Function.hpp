#pragma once
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include "CLEF.hpp"

#include "ast-nodes/Identifier.hpp"
#include "ast-nodes/Block.hpp"

#include "dyn_arr.hpp"

//!AST node representing a function signature
struct clef::Function {
   private:
      Identifier* _name;
      Type* _returnType;
      Block* _params;
   public:
      Function(Identifier* name = nullptr, Type* returnType = nullptr, Block* params = nullptr);

      Identifier* name() { return _name; }
      Type* returnType() { return _returnType; }
      Block* params() { return _params; }
      const Identifier* name() const { return _name; }
      const Type* returnType() const { return _returnType; }
      const Block* params() const { return _params; }

      bool isAnonymous() const { return _name; }
      bool sameSignature(const Function& other) const;
      bool sameSignature(const Function* other) const { return other && sameSignature(*other); }
};

#endif //FUNCTION_HPP