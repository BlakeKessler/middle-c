#ifndef FULL_TYPE_HPP
#define FULL_TYPE_HPP

#include "CLEF.hpp"

class clef::FullType {
   private:
      TypeSpec* _spec = nullptr;
      Args* _gens = nullptr;
   public:
      FullType() = default;
      FullType(TypeSpec* spec, Args* gens):_spec{spec},_gens{gens} {}

      TypeSpec* spec() { return _spec; }
      const TypeSpec* spec() const { return _spec; }
      Args* gens() { return _gens; }
      const Args* gens() const { return _gens; }

      bool operator==(const FullType& other) const { return _spec == other._spec && _gens == other._gens; }
      explicit operator bool() const { return _spec || _gens; }
};

#endif