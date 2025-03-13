#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "str_slice.hpp"

struct clef::Identifier {
   private:
      const char* _name_buf;
      const uint _name_size;
      index<Identifier> _scopeName;
      index<SpecList> _specializer;
      const KeywordID _keywordID;

   protected:
      void __printName() const;
      void __printf(const char* nodetype) const;

   public:
      static constexpr NodeType nodeType() { return NodeType::IDEN; }

      Identifier(const mcsl::str_slice name = {}, index<Identifier> scopeName = {}, index<SpecList> specializer = {}):_name_buf{name.begin()},_name_size{name.size()},_scopeName{scopeName},_specializer{specializer},_keywordID{KeywordID::_NOT_A_KEYWORD} {}
      Identifier(const KeywordID id, index<SpecList> specializer = {}):_name_buf{},_name_size{},_scopeName{},_specializer{specializer},_keywordID{id} {}

      Identifier(const Identifier& other):_name_buf{other._name_buf},_name_size{other._name_size},_scopeName{other._scopeName},_specializer{other._specializer},_keywordID{other._keywordID} {}
      Identifier& operator=(const Identifier& other) { new (this) Identifier{other}; return self; }

      index<SpecList>& specializer() { return _specializer; }
      index<const SpecList> specializer() const { return _specializer; }
      index<Identifier>& scopeName() { return _scopeName; }
      index<const Identifier> scopeName() const { return _scopeName; }

      const mcsl::str_slice name() const { return {const_cast<char*>(_name_buf), _name_size}; } //unqualified name
      KeywordID keywordID() const { return _keywordID; }

      inline bool sameScope(const Identifier& other) const { return _scopeName == other._scopeName; }
      inline bool sameName(const Identifier& other) const { return name() == other.name(); }
      bool operator==(const Identifier& other) const;

      void printf() const;
};

#endif //IDENTIFIER_HPP