#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"

struct clef::Identifier {
   private:
      const char* _name_buf;
      const uint _name_size;
      index<Identifier> _scopeName;
      const KeywordID _keywordID;
   public:
      static constexpr NodeType nodeType() { return NodeType::IDEN; }

      Identifier(const mcsl::raw_str_span name = {}, index<Identifier> scopeName = {}):_name_buf{name.begin()},_name_size{name.size()},_scopeName{scopeName},_keywordID{KeywordID::_NOT_A_KEYWORD} {}
      Identifier(const KeywordID id):_name_buf{},_name_size{},_scopeName{},_keywordID{id} {}

      Identifier(const Identifier& other):_name_buf{other._name_buf},_name_size{other._name_size},_scopeName{other._scopeName},_keywordID{other._keywordID} {}
      Identifier& operator=(const Identifier& other) { new (this) Identifier{other}; return self; }

      index<Identifier>& scopeName() { return _scopeName; }
      index<const Identifier> scopeName() const { return _scopeName; }

      const mcsl::raw_str_span name() const { return {const_cast<char*>(_name_buf), _name_size}; } //unqualified name
      KeywordID keywordID() const { return _keywordID; }

      inline bool sameScope(const Identifier& other) const { return _scopeName == other._scopeName; }
      inline bool sameName(const Identifier& other) const { return name() == other.name(); }
      bool operator==(const Identifier& other) const;
};

#endif //IDENTIFIER_HPP