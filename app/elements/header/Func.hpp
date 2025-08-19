#ifndef FUNC_HPP
#define FUNC_HPP

#include "CLEF.hpp"

#include "Symbol.hpp"

#include "dyn_arr.hpp"
#include "hash.hpp"

class clef::FuncSig {
   private:
      TypeSpec* _self_t = {};
      TypeSpec* _ret_t = {};
      mcsl::dyn_arr<TypeSpec*> _params = {};

   public:
      FuncSig(TypeSpec* self_t):_self_t(self_t) {}

      TypeSpec*& self_t() { return _self_t; }
      const TypeSpec* self_t() const { return _self_t; }

      TypeSpec*& ret_t() { return _ret_t; }
      const TypeSpec* ret_t() const { return _ret_t; }

      mcsl::arr_span<TypeSpec*> params() { return _params.span(); }
      void addParam(TypeSpec* param_t) {
         _params.push_back(param_t);
      }

      bool isMethod() { return _self_t; }

      uint64 hash(uint64 seed = mcsl::RAPIDHASH_RHS_DEFAULT) const;
      explicit operator bool() const { return _ret_t || _params.size(); }
};
class clef::Overload {
   private:
      FuncSig* _sig = {};
      mcsl::dyn_arr<Symbol*> _paramNames = {}; //should correspond to _sig
      Proc* _def = {};

   public:
      Overload() = default;
      Overload(FuncSig* sig):_sig{sig} {}
      Overload(FuncSig* sig, mcsl::dyn_arr<Symbol*>&& params, Proc* def);

      void define(mcsl::dyn_arr<Symbol*>&& params, Proc* def);

      FuncSig*& sig() { return _sig; }
      const FuncSig* sig() const { return _sig; }

      mcsl::dyn_arr<Symbol*>& paramNames() { return _paramNames; }
      const mcsl::arr_span<Symbol*> paramNames() const { return _paramNames.span(); }

      Proc*& def() { return _def; }
      const Proc* def() const { return _def; }

      bool isMethod() { return _sig->isMethod(); }

      uint64 hash(uint64 seed = mcsl::RAPIDHASH_RHS_DEFAULT) const { return _sig->hash(seed); }
      explicit operator bool() const { return _sig; }

      void checkRep() const;
};
class clef::Func {
   private:
      Symbol* _name = {};
      TypeSpec* _self_t = {}; //should match _self_t of each overload
      mcsl::dyn_arr<Overload*> _overloads = {};
   public:
      Func() = default;
      Func(Symbol* name, TypeSpec* self_t):_name{name},_self_t{self_t} {}

      res<void> registerOverload(Overload* overload);
      Overload* getOverload(FuncSig* sig);

      bool isMethod() { return _self_t; }

      void checkRep() const;
};

#endif