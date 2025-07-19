#ifndef RAW_IDENTIFIER_HPP
#define RAW_IDENTIFIER_HPP

#include "CLEF.hpp"

struct clef::RawIdentifier {
   private:
      mcsl::str_slice _name;
      index<ArgList> _specializer;
      KeywordID _kwid;
   public:
      static constexpr NodeType nodeType() { return NodeType::RAW_IDEN; }

      RawIdentifier(mcsl::str_slice name = {}, index<ArgList> specializer = {}):_name{name},_specializer{specializer},_kwid{} {}
      RawIdentifier(KeywordID id, index<ArgList> specializer = {}):_name{},_specializer{specializer},_kwid{id} {}

      RawIdentifier(const RawIdentifier& other):_name{other._name},_specializer{other._specializer},_kwid{other._kwid} {}
      RawIdentifier& operator=(const RawIdentifier& other) { return *new(this) RawIdentifier{other}; }

      const mcsl::str_slice name() const { return _name; }
      KeywordID keywordID() const { return _kwid; }
      index<ArgList>& specializer() { return _specializer; }
      index<const ArgList> specializer() const { return _specializer; }
};

namespace mcsl {
   File& write(File&, const clef::RawIdentifier&);
};

#endif //RAW_IDENTIFIER_HPP