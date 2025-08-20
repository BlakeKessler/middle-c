#ifndef FUNC_CPP
#define FUNC_CPP

#include "Func.hpp"

void clef::Overload::checkRep() const {
   if (!_sig) { //check representation of null Overload objects
      assume(!_paramNames.size());
      assume(!_def);
      return;
   }
   //check representation of non-null Overload objects
   assume(_sig->params().size() == _paramNames.size());
   for (uint i = _paramNames.size(); i--;) {
      assume(_sig->params()[i] == _paramNames[i]->type());
   }
}
void clef::Func::checkRep() const {
   for (Overload* overload : _overloads) {
      overload->checkRep();
      assume(_self_t == overload->sig()->self_t());
   }
}

uint64 clef::FuncSig::hash(uint64 seed) const {
   seed = mcsl::hash_algos::rapid_mix((uint64)_self_t, seed);
   return mcsl::hash_algos::rapid(_params.begin(), _params.size() * sizeof(TypeSpec*), seed);
}

clef::res<void> clef::Func::registerOverload(Overload* overload) {
   if (getOverload(overload->sig())) {
      return {ErrCode::DUPLICATE_OVERLOAD};
   }
   _overloads.push_back(overload);
   return {};
}
clef::Overload* clef::Func::getOverload(FuncSig* sig) {
   debug_assert(sig->self_t() == _self_t);
   for (Overload* overload : _overloads) {
      if (sig == overload->sig()) {
         return overload;
      }
   }
   return nullptr;
}

#endif