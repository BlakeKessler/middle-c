#pragma once
#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "CLEF.hpp"
#include "OpData.hpp"

struct clef::Operator {
   private:
      OpGroup _op;
   public:
      Operator():_op{} {}
      Operator(OpGroup op):_op{op} {}

      OpGroup& data() { return _op; }
      OpGroup data() const { return _op; }
};

#endif //OPERATOR_HPP