#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "CLEF.hpp"

#include "ast-nodes/Expr.hpp"

#include "map.hpp"
#include "dyn_arr.hpp"

//!TODO:
class clef::Symbol {
   public:
      enum Type : uint8 {
         null,

         EXTERN_UNSPEC,

         LABELLED_SCOPE,

         VAR,
         VAL_GENERIC,
         FUNC,
         METHOD,
         MACRO,
         ATTR,

         TYPE,
         TYPE_GENERIC,
         TYPE_EXTERN,
      };
   private:
      const mcsl::str_slice _name;
      Symbol* _parent;
      bool _parentIsExplicit;
      struct {
         mcsl::map<mcsl::str_slice, Symbol*> named;
         mcsl::dyn_arr<Symbol*> anon;
         mcsl::dyn_arr<mcsl::pair<Label, Expr*>> labels; //not common enough in practice to be worth the overhead of a hash table
         OpDefTable* ops;
      } children; //accessible child symbols
      TypeSpec* _type;

      Type _symbolType;
   public:

      TypeSpec*& type() { return _type; }
      const TypeSpec* type() const { return _type; }
      
};

#endif