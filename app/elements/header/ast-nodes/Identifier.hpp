#pragma once
#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP

#include "CLEF.hpp"

#include "SymbolNode.hpp"
#include "RawIdentifier.hpp"

#include "str_slice.hpp"

struct clef::Identifier {
   private:
      SymbolNode* _name;
      index<void> _overloadIndex;
      index<Identifier> _scopeName;
      index<ArgList> _specializer;
      KeywordID _keywordID;
      FundTypeID _id;
      QualMask _quals;

   protected:
      void __printName(mcsl::File& file) const;

   public:
      static constexpr NodeType nodeType() { return NodeType::IDEN; }

      Identifier(SymbolNode* name = {}, index<Identifier> scopeName = {}, index<ArgList> specializer = {}, QualMask quals = {}):_name{name},_overloadIndex{},_scopeName{scopeName},_specializer{specializer},_keywordID{KeywordID::_NOT_A_KEYWORD},_id{},_quals{quals} {}
      Identifier(SymbolNode* name, index<void> overloadIndex, index<Identifier> scopeName = {}, index<ArgList> specializer = {}, QualMask quals = {}):_name{name},_overloadIndex{overloadIndex},_scopeName{scopeName},_specializer{specializer},_keywordID{KeywordID::_NOT_A_KEYWORD},_id{},_quals{quals} {}
      Identifier(const KeywordID id, SymbolNode* name, index<ArgList> specializer = {}, QualMask quals = {}):_name{name},_overloadIndex{},_scopeName{},_specializer{specializer},_keywordID{id},_id{},_quals{quals} {}
      Identifier(const FundTypeID id, const KeywordID kwid, SymbolNode* name, index<ArgList> specializer = {}, QualMask quals = {}):_name{name},_overloadIndex{},_scopeName{},_specializer{specializer},_keywordID{kwid},_id{id},_quals{quals} {}
      Identifier(const RawIdentifier& other):_name{},_overloadIndex{},_scopeName{},_specializer{other.specializer()},_keywordID{other.keywordID()},_id{},_quals{} { UNREACHABLE; }

      Identifier(const Identifier& other):_name{other._name},_scopeName{other._scopeName},_specializer{other._specializer},_keywordID{other._keywordID},_id{other._id},_quals{other._quals} {}
      Identifier& operator=(const Identifier& other) { return *new (this) Identifier{other}; }

      index<ArgList>& specializer() { return _specializer; }
      index<const ArgList> specializer() const { return _specializer; }
      index<Identifier>& scopeName() { return _scopeName; }
      index<const Identifier> scopeName() const { return _scopeName; }

      const mcsl::str_slice name() const { return _name->name(); } //unqualified name
      index<void>& overloadIndex() { return _overloadIndex; }
      index<const void> overloadIndex() const { return _overloadIndex; }
      SymbolNode*& symbol() { return _name; }
      const SymbolNode* symbol() const { return _name; }
      KeywordID keywordID() const { return _keywordID; }
      FundTypeID fundTypeID() const { return _id; }

      QualMask& quals() { return _quals; }
      QualMask quals() const { return _quals; }
      void addQuals(QualMask mask) { _quals |= mask; }
      void removeQuals(QualMask mask) { _quals &= ~mask; }
      void setQualMask(QualMask mask) { _quals = mask; }

      inline bool sameScope(const Identifier& other) const { return _scopeName == other._scopeName; }
      inline bool sameName(const Identifier& other) const { return name() == other.name(); }
      bool operator==(const Identifier& other) const;
      
      void printAs(mcsl::File& file, const mcsl::str_slice nodetype) const;
};

namespace mcsl {
   File& write(File&, const clef::Identifier&);
};

#endif //IDENTIFIER_HPP