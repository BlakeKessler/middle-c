#ifndef SYMBOL_NODE_HPP
#define SYMBOL_NODE_HPP

#include "CLEF.hpp"

#include "map.hpp"
#include "dyn_arr.hpp"
#include "str_slice.hpp"

class clef::SymbolNode : private mcsl::map<mcsl::str_slice, SymbolNode*> {
   private:
      using Base_t = mcsl::map<mcsl::str_slice, SymbolNode*>;
      friend class SyntaxTree;
      
      mcsl::str_slice _name;
      mcsl::dyn_arr<mcsl::pair<mcsl::str_slice, SymbolNode*>> _aliases; //pair of name and parent scope //!TODO: maybe make this a map
      SymbolNode* _parentScope;
      SymbolType _symbolType;

      TypeSpec* _type; //defintion if _symbolType == TYPE, type if _symbolType == VAR
      mcsl::dyn_arr<mcsl::pair<TypeSpec*, index<Scope>>> _overloads; //signature, definition

      void __checkRep() { debug_assert(_symbolType == SymbolType::FUNC || !_overloads.size()); }
   public:
      SymbolNode():_name{},_aliases{},_parentScope{},_type{},_overloads{} {}
      SymbolNode(const mcsl::str_slice name, SymbolNode* parentScope):Base_t(),_name(name),_aliases{},_parentScope{parentScope},_type{},_overloads{} {}
      SymbolNode(const SymbolNode&);
      SymbolNode(SymbolNode&&);
      static SymbolNode makeIndir(SymbolNode* pointee, TypeSpec* spec);

      SymbolNode& operator=(const SymbolNode& other) { return *new (this) SymbolNode(other); }
      SymbolNode& operator=(SymbolNode&& other) { return *new (this) SymbolNode(other); }

      operator bool() const; //if anything has been done with the node

      SymbolNode* get(mcsl::str_slice name); //single-token identifier
      SymbolNode* qualifiedGet(mcsl::str_slice name); //full identifier string
      SymbolNode* qualifiedGet(mcsl::arr_span<mcsl::str_slice> name); //array of single-token identifiers making up the full identifier

      const mcsl::str_slice name() const { return _name; }
      SymbolNode* parentScope() { return _parentScope; }
      const SymbolNode* parentScope() const { return _parentScope; }

      SymbolType& symbolType() { return _symbolType; }
      SymbolType symbolType() const { return _symbolType; }

      TypeSpec* type() { return _type; }
      const TypeSpec* type() const { return _type; }
};

#endif //SYMBOL_NODE_HPP