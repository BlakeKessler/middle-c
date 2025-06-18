#ifndef SYMBOL_NODE_CPP
#define SYMBOL_NODE_CPP

#include "SymbolNode.hpp"
#include "TypeSpec.hpp"

clef::SymbolNode::SymbolNode():
   Base_t(),
   _name{},
   _aliases{},
   _parentScope{},
   _type{},
   _overloads{} {

}
clef::SymbolNode::SymbolNode(const mcsl::str_slice name, SymbolNode* parentScope):
   Base_t(),
   _name(name),
   _aliases{},
   _parentScope{parentScope},
   _symbolType{SymbolType::null},
   _type{},
   _overloads{} {

}
clef::SymbolNode::SymbolNode(mcsl::str_slice name, decltype(_aliases) aliases, SymbolNode* parentScope, SymbolType symbolType, TypeSpec* type, decltype(_overloads) overloads):
   Base_t(),
   _name{name},
   _aliases{aliases},
   _parentScope{parentScope},
   _symbolType{symbolType},
   _type{type},
   _overloads{overloads} {

}
clef::SymbolNode::SymbolNode(const SymbolNode& other):
   Base_t(other),
   _name{other._name},
   _aliases{other._aliases},
   _parentScope{other._parentScope},
   _symbolType{other._symbolType},
   _type{other._type},
   _overloads{other._overloads} {

}
clef::SymbolNode::SymbolNode(SymbolNode&& other):
   Base_t(std::move(other)),
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
   Base_t::release();
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

#endif //SYMBOL_NODE_CPP