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
      KeywordID _keywordID;
   protected:
      FundTypeID _id;
      QualMask _quals;

   protected:
      void __printName(mcsl::File& file) const;

   public:
      static constexpr NodeType nodeType() { return NodeType::IDEN; }

      Identifier(const mcsl::str_slice name = {}, index<Identifier> scopeName = {}, index<SpecList> specializer = {}, QualMask quals = {}):_name_buf{name.begin()},_name_size{name.size()},_scopeName{scopeName},_specializer{specializer},_keywordID{KeywordID::_NOT_A_KEYWORD},_id{},_quals{quals} {}
      Identifier(const KeywordID id, index<SpecList> specializer = {}, QualMask quals = {}):_name_buf{},_name_size{},_scopeName{},_specializer{specializer},_keywordID{id},_id{},_quals{quals} {}

      Identifier(const Identifier& other):_name_buf{other._name_buf},_name_size{other._name_size},_scopeName{other._scopeName},_specializer{other._specializer},_keywordID{other._keywordID},_id{other._id},_quals{other._quals} {}
      Identifier& operator=(const Identifier& other) { new (this) Identifier{other}; return self; }

      index<SpecList>& specializer() { return _specializer; }
      index<const SpecList> specializer() const { return _specializer; }
      index<Identifier>& scopeName() { return _scopeName; }
      index<const Identifier> scopeName() const { return _scopeName; }

      const mcsl::str_slice name() const { return mcsl::str_slice::make(_name_buf, _name_size); } //unqualified name
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