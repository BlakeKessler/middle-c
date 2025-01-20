#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "raw_str_span.hpp"

struct clef::Identifier {
   private:
      Identifier* _scopeName;
      const char* _name_buf;
      const uint _name_size;
      const KeywordID _keywordID;
   public:
      static constexpr NodeType nodeType() { return NodeType::IDEN; }

      Identifier(const mcsl::raw_str_span name = {}, Identifier* scopeName = {}):_scopeName{scopeName},_name_buf{name.begin()},_name_size{name.size()},_keywordID{KeywordID::_NOT_A_KEYWORD} {}
      Identifier(const KeywordID id):_scopeName{},_name_buf{},_name_size{},_keywordID{id} {}

      Identifier*& scopeName() { return _scopeName; }
      const Identifier* scopeName() const { return _scopeName; }

      const mcsl::raw_str_span name() const { return {const_cast<char*>(_name_buf), _name_size}; } //unqualified name
      KeywordID keywordID() const { return _keywordID; }

      inline bool sameScope(const Identifier& other) const { return (_scopeName == other._scopeName) || (*_scopeName == *other._scopeName); }
      inline bool sameName(const Identifier& other) const { return name() == other.name(); }
      bool operator==(const Identifier& other) const;
      
      inline operator const mcsl::raw_str_span() const { return name(); }
};

#endif //IDENTIFIER_HPP