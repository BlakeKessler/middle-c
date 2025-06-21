#ifndef SYMBOL_NODE_CPP
#define SYMBOL_NODE_CPP

#include "SymbolNode.hpp"
#include "TypeSpec.hpp"

clef::SymbolNode::SymbolNode():
   _childSymbols(),
   _name{},
   _aliases{},
   _parentScope{},
   _type{},
   _overloads{} {

}
clef::SymbolNode::SymbolNode(const mcsl::str_slice name, SymbolNode* parentScope, TypeSpec* typespec, SymbolType symbType):
   _childSymbols(),
   _name(name),
   _aliases{},
   _parentScope{parentScope},
   _symbolType{symbType},
   _type{typespec},
   _overloads{} {

}
clef::SymbolNode::SymbolNode(mcsl::str_slice name, decltype(_aliases) aliases, SymbolNode* parentScope, SymbolType symbolType, TypeSpec* type, decltype(_overloads) overloads):
   _childSymbols(),
   _name{name},
   _aliases{aliases},
   _parentScope{parentScope},
   _symbolType{symbolType},
   _type{type},
   _overloads{overloads} {

}
clef::SymbolNode::SymbolNode(const SymbolNode& other):
   _childSymbols(other),
   _name{other._name},
   _aliases{other._aliases},
   _parentScope{other._parentScope},
   _symbolType{other._symbolType},
   _type{other._type},
   _overloads{other._overloads} {

}
clef::SymbolNode::SymbolNode(SymbolNode&& other):
   _childSymbols(std::move(other._childSymbols)),
   _name{other._name},
   _aliases{std::move(other._aliases)},
   _parentScope{other._parentScope},
   _symbolType{other._symbolType},
   _type{other._type},
   _overloads{std::move(other._overloads)} {
      if (this != &other) {
         other.release();
      }
}

void clef::SymbolNode::release() {
   _childSymbols.release();
   _aliases.release();
   _overloads.release();
}

clef::SymbolNode clef::SymbolNode::makeIndir(SymbolNode* pointee, TypeSpec* spec) {
   debug_assert(pointee && isType(pointee->symbolType()));
   return {
      pointee->_name,
      {},
      pointee,
      SymbolType::INDIR,
      spec,
      {}
   };
}

void clef::SymbolNode::__checkRep() {
   debug_assert(_symbolType == SymbolType::FUNC || !_overloads.size());
   debug_assert(_symbolType != SymbolType::INDIR || (
      _name == _parentScope->_name &&
      !_aliases.size() &&
      _type->metaType() == TypeSpec::INDIR &&
      _type->pointee() == _parentScope->type()
   ));
}

clef::SymbolNode::operator bool() const {
   return _overloads.size() || _aliases.size() || _symbolType != SymbolType::null;
}

clef::SymbolNode** clef::SymbolNode::find(mcsl::str_slice name) {
   if (SymbolNode** node = _childSymbols.find(name); node) { return node; }
   
   if (_parentScope) {
      SymbolNode** node = _parentScope->find(name);
      if (node) { return node; }
   }

   return nullptr;
}
clef::SymbolNode* clef::SymbolNode::get(mcsl::str_slice name) {
   SymbolNode** node = find(name);
   if (node) {
      return *node;
   }
   return nullptr;
}
clef::SymbolNode*& clef::SymbolNode::operator[](mcsl::str_slice name) {
   SymbolNode** node = find(name);
   if (node) {
      return *node;
   }
   return _childSymbols[name];
}
void clef::SymbolNode::insert(SymbolNode* symbol) {
   operator[](symbol->name()) = symbol;
}

#endif //SYMBOL_NODE_CPP