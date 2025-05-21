#pragma once
#ifndef FUND_TYPE_HPP
#define FUND_TYPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"
#include "pretty-print.hpp"

struct clef::FundamentalType : public clef::Type {
   public:
      static constexpr NodeType nodeType() { return NodeType::FUND_TYPE; }

      FundamentalType():Type{} {}
      FundamentalType(FundTypeID id):Type{} { _id = id; }
      FundamentalType(FundTypeID id, const Type& name):Type{name} { _id = id; }
      FundamentalType(FundTypeID id, index<SpecList> spec, QualMask quals):Type{{}, {}, spec, quals} { _id = id; }

      FundTypeID& id() { return _id; }
      FundTypeID id() const { return _id; }
};

namespace mcsl {
   File& write(File&, const clef::FundType&);
};

#endif //FUND_TYPE_HPP