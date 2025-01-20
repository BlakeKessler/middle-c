#ifndef KEYWORD_HPP
#define KEYWORD_HPP

#include "CLEF.hpp"

struct clef::Keyword {
   private:
      KeywordID _id;
   public:
      static constexpr NodeType nodeType() { return NodeType::KEYWORD; }
      
      constexpr Keyword(const KeywordID id = {}):_id{id} {}

      constexpr KeywordID id() const { return _id; }
      constexpr KeywordID& id() { return _id; }

      constexpr operator KeywordID() const { return _id; }
};

#endif //KEYWORD_HPP