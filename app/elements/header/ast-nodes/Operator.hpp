#pragma once
#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include "CLEF.hpp"

struct clef::Operator {
   private:
      OperatorID _id;
   public:
      Operator():_id{} {}
      Operator(const OperatorID id):_id{id} {}
      Operator(const OpData);

      OperatorID& id() { return _id; }
      OperatorID id() const { return _id; }

      constexpr bool isDecl() const { return _id == OperatorID::DECL; }

      constexpr bool isForLoop() const { return _id == OperatorID::FOR; }
      constexpr bool isForeachLoop() const { return _id == OperatorID::FOREACH; }
      constexpr bool isWhileLoop() const { return _id == OperatorID::WHILE; }
      constexpr bool isDoWhileLoop() const { return _id == OperatorID::DO_WHILE; }

      constexpr bool isLoop() const { return +(_id & OperatorID::_LOOPS); }
      constexpr bool isSimpleLoop() const { return isLoop() && !isForLoop() && !isForeachLoop(); }

      constexpr bool isIf() const { return _id == OperatorID::IF; }
      constexpr bool isElse() const { return _id == OperatorID::ELSE; }
      constexpr bool isElseIf() const { return _id == OperatorID::ELIF; }
      constexpr bool isSwitch() const { return _id == OperatorID::SWITCH; }
      constexpr bool isMatch() const { return _id == OperatorID::MATCH; }
};

#endif //OPERATORS_HPP