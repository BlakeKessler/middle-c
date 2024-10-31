#pragma once
#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "CLEF.hpp"

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

      bool hasProps(const SymbolProp props) const { return (bool)(_props & props); }

      sint operator<=>(const Symbol& other) const { return _name <=> other._name; }
      sint operator== (const Symbol& other) const { return _name ==  other._name; }
      sint operator!= (const Symbol& other) const { return _name !=  other._name; }
      template <mcsl::str_t str_t> sint operator<=>(const str_t& otherName) const { return _name <=> otherName; }
      template <mcsl::str_t str_t> sint operator== (const str_t& otherName) const { return _name ==  otherName; }
      template <mcsl::str_t str_t> sint operator!= (const str_t& otherName) const { return _name !=  otherName; }

      template<mcsl::str_t str_t> Symbol& operator[](const str_t& str) { return (*_table)[str]; }
      template<mcsl::str_t str_t> Symbol& at(const str_t& str);
      template<mcsl::str_t str_t> const Symbol& operator[](const str_t& str) const { return (*_table)[str]; }
      template<mcsl::str_t str_t> const Symbol& at(const str_t& str) const;
};

#endif //SYMBOL_HPP