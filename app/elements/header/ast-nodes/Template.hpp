#pragma once
#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include "CLEF.hpp"

#include "ast-nodes/Block.hpp"

struct clef::Template {
   private:
      Block* _params;
      astNode* _target;
   public:
      Template():_params{},_target{} {}
      Template(Block* params, astNode* target):_params{params},_target{target} {}

      Block*& params() { return _params; }
      astNode*& target() { return _target; }
      const Block* params() const { return _params; }
      const astNode* target() const { return _target; }
};

#endif //TEMPLATE_HPP