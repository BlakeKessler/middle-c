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
   #define FIND(cond) \
      for (entry e : candidates) {                            \
         auto [sig, _] = e.function->getOverload(e.overload); \
         debug_assert(sig->metaType() == TypeSpec::FUNC_SIG); \
         auto params = sig->funcSig().params.span();          \
         if (cond) {                                          \
            return e;                                         \
         }                                                    \
      }

   if (lhs && rhs) { //binary
      FIND(params.size() == 2 && params[0].first == lhs && params[1].first == rhs);
   } else { //unary
      FIND(params.size() == 1 && params[0].first == lhs);
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
}

#endif