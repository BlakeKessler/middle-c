#pragma once
#ifndef FUND_TYPE_HPP
#define FUND_TYPE_HPP

#include "CLEF.hpp"
#include "ast-nodes/Type.hpp"

struct clef::FundamentalType : public clef::Type {
   private:
      FundTypeID _id;
   public:
      static constexpr NodeType nodeType() { return NodeType::FUND_TYPE; }

      FundamentalType():Type{},_id{} {}
      FundamentalType(FundTypeID id):Type{},_id{id} {}
      FundamentalType(FundTypeID id, const char* str, const uint len):Type{str, len},_id{id} {}

      FundTypeID& id() { return _id; }
      FundTypeID id() const { return _id; }
};

#endif //FUND_TYPE_HPP