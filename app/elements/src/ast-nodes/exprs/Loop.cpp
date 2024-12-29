#ifndef LOOP_CPP
#define LOOP_CPP

#include "ast-nodes/exprs/Loop.hpp"
#include "ast-nodes/Operator.hpp"
#include "ast-nodes/ForLoopParams.hpp"

clef::Expr*& clef::Loop::condition() {
   assert(_op);
   if (_op->isSimpleLoop()) {
      return (Expr*&)_lhs;
   } else {
      if (_op->isForLoop()) {
         return ((ForLoopParams*)_lhs)->condition();
      } else {
         throwError(ErrCode::BAD_NODE_CAST, "attempt to access condition of foreach loop");
      }
   }
}
const clef::Expr* clef::Loop::condition() const {
   assert(_op);
   if (_op->isSimpleLoop()) {
      return (Expr*)_lhs;
   } else {
      if (_op->isForLoop()) {
         return ((ForLoopParams*)_lhs)->condition();
      } else {
         throwError(ErrCode::BAD_NODE_CAST, "attempt to access condition of foreach loop");
      }
   }
}

clef::ForLoopParams*& clef::Loop::forParams() {
   assert(_op);
   if (_op->isForLoop()) {
      return (ForLoopParams*&)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}
const clef::ForLoopParams* clef::Loop::forParams() const {
   assert(_op);
   if (_op->isForLoop()) {
      return (ForLoopParams*)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}

clef::ForeachLoopParams*& clef::Loop::foreachParams() {
   assert(_op);
   if (_op->isForeachLoop()) {
      return (ForeachLoopParams*&)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}
const clef::ForeachLoopParams* clef::Loop::foreachParams() const {
   assert(_op);
   if (_op->isForeachLoop()) {
      return (ForeachLoopParams*)_lhs;
   } else {
      throwError(ErrCode::BAD_NODE_CAST, "attempt to access ForLoopParams of non-for loop");
   }
}

#endif //LOOP_CPP