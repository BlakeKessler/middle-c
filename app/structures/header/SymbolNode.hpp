#ifndef SYMBOL_NODE_HPP
#define SYMBOL_NODE_HPP

#include "CLEF.hpp"

#include "map.hpp"
#include "dyn_arr.hpp"
#include "str_slice.hpp"

class clef::SymbolNode {
   private:
      mcsl::map<mcsl::str_slice, SymbolNode*> _childSymbols;
      mcsl::dyn_arr<SymbolNode*> _anonChildren; //accessible anonymous children (subtypes)
      mcsl::dyn_arr<SymbolNode*> _subScopes; //inaccessible anonymous children (funcs, macros, conditionals, loops)

      mcsl::str_slice _name;
      mcsl::dyn_arr<mcsl::pair<mcsl::str_slice, SymbolNode*>> _aliases; //pair of name and parent scope //!TODO: maybe make this a map
      SymbolNode* _parentScope;
      SymbolType _symbolType;

      TypeSpec* _type; //defintion if _symbolType == TYPE, type if _symbolType == VAR
      mcsl::dyn_arr<mcsl::pair<TypeSpec*, index<FuncDef>>> _overloads; //signature, definition

      void __checkRep();

      SymbolNode(mcsl::str_slice, decltype(_aliases), SymbolNode*, SymbolType, TypeSpec*, const decltype(_overloads)&);
      SymbolNode(mcsl::str_slice, decltype(_aliases), SymbolNode*, SymbolType, TypeSpec*, decltype(_overloads)&&);
   public:
      SymbolNode();
      SymbolNode(const mcsl::str_slice name, SymbolNode* parentScope, TypeSpec* typespec, SymbolType symbType);
      SymbolNode(const SymbolNode&);
      SymbolNode(SymbolNode&&);
      static SymbolNode makeIndir(SymbolNode* pointee, TypeSpec* spec);

      void release();

      SymbolNode& operator=(const SymbolNode& other) { return *new (this) SymbolNode(other); }
      SymbolNode& operator=(SymbolNode&& other) { return *new (this) SymbolNode(std::move(other)); }

      void setSymbolType(SymbolType symbolType) { _symbolType = symbolType; }
      void setType(TypeSpec* type) { _type = type;}

      explicit operator bool() const; //if anything has been done with the node

      SymbolNode** find(mcsl::str_slice name);
      SymbolNode* get(mcsl::str_slice name); //single-token identifier
      SymbolNode* qualifiedGet(mcsl::str_slice name); //full identifier string
      SymbolNode* qualifiedGet(mcsl::arr_span<mcsl::str_slice> name); //array of single-token identifiers making up the full identifier
      SymbolNode*& operator[](mcsl::str_slice name);
      void insert(SymbolNode*);

      auto begin() { return _childSymbols.begin(); }
      auto begin() const { return _childSymbols.begin(); }
      auto end() { return _childSymbols.end(); }
      auto end() const { return _childSymbols.end(); }

      const mcsl::str_slice name() const { return _name; }
      SymbolNode* parentScope() { return _parentScope; }
      const SymbolNode* parentScope() const { return _parentScope; }

      SymbolType& symbolType() { return _symbolType; }
      SymbolType symbolType() const { return _symbolType; }

      TypeSpec* type() { return _type; }
      const TypeSpec* type() const { return _type; }

      mcsl::pair<index<void>, bool> registerOverload(TypeSpec* signature); //index, isNew
      index<FuncDef> defineOverload(index<void> i, index<FuncDef> def); //returns previous definition
      auto& getOverload(uint i) { return _overloads[i]; }
      mcsl::arr_span<mcsl::pair<TypeSpec*, index<FuncDef>>> overloads() { return _overloads.span(); }
      const mcsl::arr_span<mcsl::pair<TypeSpec*, index<FuncDef>>> overloads() const { return _overloads.span(); }
};

/* |===============|
 * | SPECIAL CASES |
 * |===============|
 * _symbolType == INDIR -> pointee type in _parentScope
 */

#endif //SYMBOL_NODE_HPP