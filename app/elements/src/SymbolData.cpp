#ifndef SYMBOL_DATA_CPP
#define SYMBOL_DATA_CPP

#include "SymbolData.hpp"

template <mcsl::str_t str_t> clef::SymbolData::SymbolData(const str_t& name, const SymbolProp props, SymbolTable* internalSymbols):
_name(name),_props(props),_table(internalSymbols) {
   //if no table is provided, but the props indicate there should be one, allocate and initialize one
   if (!_table && (_props & SymbolProp::HAS_NAME_TABLE)) {
      _table = mcsl::malloc<SymbolTable>(1);
      *_table = SymbolTable();
   }
}
clef::SymbolData::~SymbolData() {
   mcsl::free(_table);
}
//!updates allocation of internal name table based on assigned properties
//!returns whether or not any changes were made
bool clef::SymbolData::update_alloc() {
   if ((bool)(_props & SymbolProp::HAS_NAME_TABLE)) {
      //allocate table if is not yet allocated but should be
      if (!_table) {
         _table = mcsl::malloc<SymbolTable>(1);
         *_table = SymbolTable();
         return true;
      }
   } else {
      //release table if it is allocated but should not be
      if (_table) {
         mcsl::free(_table);
         return true;
      }
   }

   return false;
}

//!access internal name table
template<mcsl::str_t str_t> clef::SymbolData& clef::SymbolData::operator[](const str_t& str) {
   if (!(_props & SymbolProp::HAS_NAME_TABLE)) {
      mcsl::mcsl_throw(mcsl::ErrCode::SEGFAULT, "Symbol Table Entry for '%.*s' does not have a Symbol Table", str.size(), str.begin());
   }
   return (*_table)[str];
}
//!access internal name table
template<mcsl::str_t str_t> const clef::SymbolData& clef::SymbolData::operator[](const str_t& str) const {
   if (!(_props & SymbolProp::HAS_NAME_TABLE)) {
      mcsl::mcsl_throw(mcsl::ErrCode::SEGFAULT, "Symbol Table Entry for '%.*s' does not have a Symbol Table", str.size(), str.begin());
   }
   return (*_table)[str];
}

#endif //SYMBOL_DATA_CPP