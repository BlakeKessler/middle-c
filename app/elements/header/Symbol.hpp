#pragma once
#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "CLEF.hpp"
#include "SymbolTable.hpp"

#include "string.hpp"

struct clef::Symbol {
   private:
      const mcsl::string _name;
      SymbolProp _props;
      SymbolTable* _table;
   public:
      Symbol();
      template<mcsl::str_t str_t> Symbol(const str_t& name, const SymbolProp props = SymbolProp::NIL, SymbolTable* childSymbols = nullptr);
      ~Symbol();
      bool update_alloc();

      const mcsl::string& name() const { return _name; }
      SymbolProp props() const { return _props; }

      SymbolProp setProps(const SymbolProp props) { _props = _props | props; return _props; }
      SymbolProp unsetProps(const SymbolProp props) { _props = _props & ~props; return _props; }
      SymbolProp maskProps(const SymbolProp props) { _props = _props & props; return _props; }
      SymbolProp toggleProps(const SymbolProp props) { _props = _props ^ props; return _props; }

      bool hasProps(const SymbolProp props) { return (bool)(_props & props); }

      bool operator<=>(const Symbol& other) { return _name <=> other._name; }
      template <mcsl::str_t str_t> bool operator<=>(const str_t& otherName) { return _name <=> otherName; }

      template<mcsl::str_t str_t> Symbol& operator[](const str_t& str);
      template<mcsl::str_t str_t> const Symbol& operator[](const str_t& str) const;
};

#endif //SYMBOL_HPP