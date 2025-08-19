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
         mcsl::dyn_arr<Symbol*> membs;
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
      union M {
         FundTypeID fund;

         Tuple tup;
         Trait trait;
         Namespace ns;
         Obj obj;
         Indir indir;

         Func* func;
         FuncSig* sig;

         ~M() {}
      } m;
      Metatype _type;

   public:
      ~TypeSpec() {
         switch (_type) {
            case null     : break;
            case FUND     : break;
            case TUPLE    : std::destroy_at(&m.tup); break;
            case TRAIT    : std::destroy_at(&m.trait); break;
            case NAMESPACE: std::destroy_at(&m.ns); break;
            case OBJ      : std::destroy_at(&m.obj); break;
            case INDIR    : std::destroy_at(&m.indir); break;
            case FUNC     : break;
            case SIG      : break;
         }
      }
};

#endif