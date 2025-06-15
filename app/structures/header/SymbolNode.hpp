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
      index<astNode> _decl;
      index<astNode> _def;
      mcsl::dyn_arr<mcsl::pair<mcsl::str_slice, SymbolNode*>> _aliases; //pair of name and parent scope
      SymbolNode* _parentScope;
   public:
      SymbolNode():_name{},_decl{},_def{},_aliases{},_parentScope{} {}
      SymbolNode(const mcsl::str_slice name, SymbolNode* parentScope):Base_t(),_name(name),_decl{},_def{},_aliases{},_parentScope{parentScope} {}

      operator bool() const; //if anything has been done with the node

      SymbolNode* get(mcsl::str_slice name); //single-token identifier
      SymbolNode* qualifiedGet(mcsl::str_slice name); //full identifier string
      SymbolNode* qualifiedGet(mcsl::arr_span<mcsl::str_slice> name); //array of single-token identifiers making up the full identifier

      const mcsl::str_slice name() const { return _name; }
      SymbolNode* parentScope() { return _parentScope; }
};

#endif //SYMBOL_NODE_HPP