#ifndef LOOP_CPP
#define LOOP_CPP

#include "ast-nodes/exprs/Loop.hpp"
#include "ast-nodes/ForLoopParams.hpp"

clef::Expr*& clef::Loop::condition() {
   if (isSimpleLoop(_op)) {
      return (Expr*&)_lhs;
   } else {
      if (isForLoop(_op)) {
         return ((ForLoopParams*)_lhs)->condition();
      } else {
         throwError(ErrCode::BAD_NODE_CAST, "attempt to access condition of foreach loop");
      }
   }
}
const clef::Expr* clef::Loop::condition() const {
   if (isSimpleLoop(_op)) {
      return (Expr*)_lhs;
   } else {
      if (isForLoop(_op)) {
         return ((ForLoopParams*)_lhs)->condition();
      } else {
         throwError(ErrCode::BAD_NODE_CAST, "attempt to access condition of foreach loop");
      }
   }
}

clef::ForLoopParams*& clef::Loop::forParams() {
   if (isForLoop(_op)) {
      return (ForLoopParams*&)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}
const clef::ForLoopParams* clef::Loop::forParams() const {
   if (isForLoop(_op)) {
      return (ForLoopParams*)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}

clef::ForeachLoopParams*& clef::Loop::foreachParams() {
   if (isForeachLoop(_op)) {
      return (ForeachLoopParams*&)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}
const clef::ForeachLoopParams* clef::Loop::foreachParams() const {
   if (isForeachLoop(_op)) {
      return (ForeachLoopParams*)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}

#endif //LOOP_CPP