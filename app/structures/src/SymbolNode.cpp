#ifndef SYMBOL_NODE_CPP
#define SYMBOL_NODE_CPP

#include "SymbolNode.hpp"
#include "TypeSpec.hpp"

clef::SymbolNode::SymbolNode():
   _childSymbols(),
   _name{},
   _aliases{},
   _parentScope{},
   _symbolType{SymbolType::null},
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
   _childSymbols(other._childSymbols),
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
   _anonChildren.release();
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
   return _overloads.size() || _aliases.size() || (_symbolType != SymbolType::null && _symbolType != SymbolType::EXTERN_IDEN && _symbolType != SymbolType::EXTERN_TYPE);
}

clef::SymbolNode** clef::SymbolNode::find(mcsl::str_slice name) { //!TODO: should probably be searching only parents and anonymous children
   if (SymbolNode** node = _childSymbols.find(name); node) {
      return node;
   }
   
   if (_parentScope) {
      if (SymbolNode** node = _parentScope->find(name); node) {
         return node;
      }
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
   if (SymbolNode** node = find(name); node) {
      return *node;
   }
   return _childSymbols[name];
}
void clef::SymbolNode::insert(SymbolNode* symbol) {
   operator[](symbol->name()) = symbol;
}

mcsl::pair<clef::index<void>, bool> clef::SymbolNode::registerOverload(TypeSpec* signature) {
   for (uint i = 0; i < _overloads.size(); ++i) {
      if (*_overloads[i].first == *signature) {
         return {i, false};
      }
   }
   mcsl::pair<index<void>, bool> ret = {_overloads.size(), true};
   _overloads.emplace_back(signature, 0);
   return ret;
}
clef::index<clef::FuncDef> clef::SymbolNode::defineOverload(index<void> i, index<FuncDef> def) {
   index<FuncDef> tmp = _overloads[i].second;
   _overloads[i].second = def;
   return tmp;
}

#endif //SYMBOL_NODE_CPP