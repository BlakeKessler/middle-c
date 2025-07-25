#ifndef OP_DEF_TABLE_CPP
#define OP_DEF_TABLE_CPP

#include "OpDefTable.hpp"

#include "SymbolNode.hpp"
#include "TypeSpec.hpp"

clef::OpDefTable::entry clef::OpDefTable::get(OpID op, TypeSpec* lhs, TypeSpec* rhs) {
   auto candidates = _buf[toIndex(op)][subtableIndex(lhs, rhs)].span();
   if (!lhs) { //postfix operators
      lhs = rhs;
      rhs = nullptr;
   }
   #define FIND(argc1, cond1, argc2, cond2) \
      for (entry e : candidates) {                               \
         auto [s, _] = e.function->getOverload(e.overload);      \
         debug_assert(s->metaType() == TypeSpec::FUNC_SIG);      \
         auto& sig = s->funcSig();                               \
         auto params = sig.params.span();                        \
         if (!sig.selfType && params.size() == argc1 && cond1) { \
            return e;                                            \
         }                                                       \
         if ( sig.selfType && params.size() == argc2 && cond2) { \
            return e;                                            \
         }                                                       \
      }

   if (lhs && rhs) { //binary
      FIND(
         2, params[0].first == lhs && params[1].first == rhs,
         1,    sig.selfType == lhs && params[0].first == rhs
      );
   } else { //unary
      debug_assert(lhs && !rhs);
      FIND(
         1, params[0].first == lhs,
         0,    sig.selfType == lhs
      );
   }
   #undef FIND
   return {0, 0};
}
//!HACK: uses const_cast
clef::OpDefTable::const_entry clef::OpDefTable::get(OpID op, TypeSpec* lhs, TypeSpec* rhs) const {
   return const_cast<OpDefTable*>(this)->get(op, lhs, rhs);
}

//returns whether a new element was inserted
bool clef::OpDefTable::insert(SymbolNode* f, index<void> i, OpID op, TypeSpec* lhs, TypeSpec* rhs) {
   if (get(op, lhs, rhs)) {
      return false;
   } else {
      insertUnchecked(f, i, op, lhs, rhs);
      return true;
   }
}

void clef::OpDefTable::insertUnchecked(SymbolNode* f, index<void> i, OpID op, TypeSpec* lhs, TypeSpec* rhs) {
   _buf[toIndex(op)][subtableIndex(lhs, rhs)].emplace_back(f, i);
   debug_assert(_buf[toIndex(op)][subtableIndex(lhs,rhs)].size());
   debug_assert(_buf[toIndex(op)][subtableIndex(lhs,rhs)].back().function == f);
   debug_assert(get(op, lhs, rhs));
}

#endif