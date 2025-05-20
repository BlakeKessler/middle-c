#pragma once
#ifndef OP_PRECS_HPP
#define OP_PRECS_HPP

#include "CLEF.hpp"

#include "OpData.hpp"

#include "static_arr.hpp"

static_assert(sizeof(clef::OpID) == 1);

struct clef::OpPrecs {
   private:
      ubyte _precs[256][4];

      constexpr ubyte makeFlags(OpProps props);
   public:
      template <mcsl::is_t<clef::OpData>... Argv_t> constexpr OpPrecs(const Argv_t... initList);

      constexpr ubyte getPrec(OpID id, ubyte flags) const { return _precs[+id][flags]; }
      constexpr ubyte getPrec(OpID id, bool hasLHS, bool hasRHS) const { return getPrec(id, hasLHS | (hasRHS << 1)); }

      constexpr ubyte operator()(OpID id, ubyte flags) const { return getPrec(id, flags); }
      constexpr ubyte operator()(OpID id, bool hasLHS, bool hasRHS) const { return getPrec(id, hasLHS, hasRHS); }
};

namespace clef {
   constexpr OpPrecs GetAllOpPrecsData();
};

#include "../src/OpPrecs.cpp"

#endif //OP_PRECS_HPP