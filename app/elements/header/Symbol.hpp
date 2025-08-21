#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "CLEF.hpp"

#include "ast-nodes/Expr.hpp"
#include "FullType.hpp"

#include "map.hpp"
#include "dyn_arr.hpp"

class clef::Symbol {
   public:
      enum Type : uint8 {
         null,

         EXTERN_UNSPEC,

         LABELED_SCOPE,

         VAR,
         VAL_GENERIC,
         FUNC,
         METHOD,
         MACRO,
         ATTR,

         __type_bit = 8_m,
         TYPE_EXTERN,
         TYPE_GENERIC,
         CLASS,
         STRUCT,
         TRAIT,
         UNION,
         ENUM,
         ENUM_UNION,
         MASK,
         NAMESPACE,
         TUPLE,
      };
      static bool isType(Type t) { return t & __type_bit; }
   private:
      const mcsl::str_slice _name;
      Args* _gens;
      struct {
         Symbol* symbol;
         Args* gens; //should only be non-null if `symbol` is non-null
      } _parent;
      struct {
         mcsl::map<mcsl::str_slice, Symbol*> named;
         mcsl::dyn_arr<Symbol*> anon;
         mcsl::dyn_arr<mcsl::pair<Label, Expr*>> labels; //not common enough in practice to be worth the overhead of a hash table
         OpDefTable* ops;
      } _children; //accessible child symbols
      FullType _type;
      
      Type _symbolType;
   public:
      Symbol(Type t, Identifier name);

      void setGenParams(Args* params) { _gens = params; }
      bool isGeneric() const { return _gens; }

      Type symbolType() const { return _symbolType; }

      res<Symbol*> get(const mcsl::str_slice name);
      res<Symbol*> insert(Symbol* symbol);

      auto type() { return _type; }
      void setType(FullType type) {
         debug_assert(!_type);
         _type = type;
         debug_assert(_type);
      }
      inline void setType(Identifier type) { return setType(type.symbol->_type); }
      
};

#endif