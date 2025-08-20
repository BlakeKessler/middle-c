#ifndef TYPE_SPEC_HPP
#define TYPE_SPEC_HPP

#include "CLEF.hpp"

#include "Func.hpp"

#include "dyn_arr.hpp"

class clef::TypeSpec {
   public:
      enum Metatype {
         null,
         
         FUND,

         TUPLE,
         TRAIT,
         NAMESPACE,
         OBJ,
         
         INDIR,
         
         FUNC,
         SIG,
      };
      
      struct Tuple {
         mcsl::dyn_arr<Identifier> membs;
      };
      struct Trait {
         mcsl::dyn_arr<Func*> funcs;
         mcsl::dyn_arr<Func*> methods;
         mcsl::dyn_arr<TypeSpec*> subtypes;
      };
      struct Namespace {
         mcsl::dyn_arr<Symbol*> membs;
         mcsl::dyn_arr<Func*> funcs;
         mcsl::dyn_arr<TypeSpec*> subtypes;
      };
      struct Obj {
         Tuple data;
         Tuple staticData;
         Trait funcs;
      };
      struct Indir {
         enum IndType : uint8 {
            PTR,
            REF,
            SLICE,
            ARR
         };
         TypeSpec* pointee;
         IndType indirType;
         uint bounds; //indirType == ARR && bounds == 0 → bounds are auto
      };

   private:
      Symbol* _canonName;
      union M {
         FundTypeID fund;

         Tuple tup;
         Trait trait;
         Namespace ns;
         Obj obj;
         Indir indir;

         Func* fn;
         FuncSig* sig;

         ~M() {} //trivial destructor - deallocations handled in supertype
      } m = {.obj = {}}; static_assert(sizeof(Obj) == sizeof(M));
      Metatype _type;

      TypeSpec(Symbol* canonName, Metatype t): _canonName{canonName},_type{t} {}
   public:
      static TypeSpec makeTuple(Symbol* canonName) { return {canonName, TUPLE}; }

      ~TypeSpec() {
         switch (_type) {
            case null     : break;
            case FUND     : break;
            case TUPLE    : std::destroy_at(&m.tup  ); break;
            case TRAIT    : std::destroy_at(&m.trait); break;
            case NAMESPACE: std::destroy_at(&m.ns   ); break;
            case OBJ      : std::destroy_at(&m.obj  ); break;
            case INDIR    : std::destroy_at(&m.indir); break;
            case FUNC     : break;
            case SIG      : break;
         }
      }

      Symbol* canonName() { return _canonName; }
      const Symbol* canonName() const { return _canonName; }
      void setCanonName(Symbol* s) { debug_assert(!_canonName); _canonName = s; }

      Metatype metaType() const { return _type; }

      #define GETTERS(name, METATYPE) \
         auto& name() { assume(_type == METATYPE); return m.name; } \
         const auto& name() const { assume(_type == METATYPE); return m.name; }
      GETTERS(fund, FUND)
      GETTERS(tup, TUPLE)
      GETTERS(trait, TRAIT)
      GETTERS(ns, NAMESPACE)
      GETTERS(obj, OBJ)
      GETTERS(indir, INDIR)
      GETTERS(fn, FUNC)
      GETTERS(sig, SIG)
      #undef GETTERS

};

#endif