#pragma once
#ifndef TYPE_SPEC_HPP
#define TYPE_SPEC_HPP

#include "CLEF.hpp"

#include "ast-nodes/Object.hpp"
#include "ast-nodes/Function.hpp"

#include "dyn_arr.hpp"

struct clef::TypeSpec {
   private:
      mcsl::dyn_arr<Object> _dataMembers;
      mcsl::dyn_arr<Function> _memberFuncs;

      mcsl::dyn_arr<Object> _staticObjs;
      mcsl::dyn_arr<Function> _staticFuncs;
   public:
};

#endif //TYPE_SPEC_HPP