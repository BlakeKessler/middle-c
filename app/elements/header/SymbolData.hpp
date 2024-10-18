#pragma once
#ifndef SYMBOL_DATA_HPP
#define SYMBOL_DATA_HPP

#include "CLEF.hpp"
#include "SymbolTable.hpp"

#include "string.hpp"

struct clef::SymbolData {
   private:
      const mcsl::string _name;
      SymbolProp _props;
      SymbolTable* _table;
   public:
      SymbolData();
      template<mcsl::str_t str_t> SymbolData(const str_t& name, const SymbolProp props = SymbolProp::NIL, SymbolTable* childSymbols = nullptr);
      ~SymbolData();
      bool update_alloc();

      const mcsl::string& name() const { return _name; }
      SymbolProp props() const { return _props; }

      SymbolProp setProps(const SymbolProp props) { _props = _props | props; return _props; }
      SymbolProp unsetProps(const SymbolProp props) { _props = _props & ~props; return _props; }
      SymbolProp maskProps(const SymbolProp props) { _props = _props & props; return _props; }
      SymbolProp toggleProps(const SymbolProp props) { _props = _props ^ props; return _props; }

      bool hasProps(const SymbolProp props) { return (bool)(_props & props); }

      bool operator<=>(const SymbolData& other) { return _name <=> other._name; }
      template <mcsl::str_t str_t> bool operator<=>(const str_t& otherName) { return _name <=> otherName; }

      template<mcsl::str_t str_t> SymbolData& operator[](const str_t& str);
      template<mcsl::str_t str_t> const SymbolData& operator[](const str_t& str) const;
};

#endif //SYMBOL_DATA_HPP