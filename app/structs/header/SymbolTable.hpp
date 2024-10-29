#pragma once
#ifndef NAME_TABLE_HPP
#define NAME_TABLE_HPP

#include "CLEF.hpp"
#include "Symbol.hpp"
#include "astNode.hpp"

#include "alloc.hpp"
#include <unordered_map>

struct clef::SymbolTable {
   private:
      std::unordered_map<mcsl::string, Symbol> _names;
   public:
      SymbolTable();
      SymbolTable(const SymbolTable& other);

      template<mcsl::str_t str_t, mcsl::is_t<mcsl::container_base<str_t>> container_t> Symbol& operator[](const container_t&);
      template<mcsl::str_t str_t> Symbol& operator[](const str_t& tok);
      // Symbol& operator[](const astNode& node) { return self[node.token()]; }


      // SymbolTable(): _names({}) {}
      // SymbolTable(const SymbolTable& other): _names(other._names) {}

      // template<mcsl::str_t str_t, mcsl::is_t<mcsl::container_base<str_t>> container_t> Symbol& operator[](const container_t&);
      // template<mcsl::str_t str_t> Symbol& operator[](const str_t& tok) { return _names[tok]; }
      // // Symbol& operator[](const astNode& node) { return self[node.token()]; }
};

#endif //NAME_TABLE_HPP