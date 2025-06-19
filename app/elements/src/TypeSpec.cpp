#ifndef TYPE_SPEC_CPP
#define TYPE_SPEC_CPP

#include "TypeSpec.hpp"

clef::TypeSpec::TypeSpec(FundTypeID id):
   _metatype{FUND_TYPE},
   _fund{.id = id} {

}

clef::TypeSpec::~TypeSpec() {
   switch (_metatype) {
      case FUND_TYPE:
         break;

      case INDIR:
         std::destroy_at(&_indir.table);
         break;

      case COMPOSITE:
         std::destroy_at(&_composite.tpltParams);
         std::destroy_at(&_composite.parentTypes);
         std::destroy_at(&_composite.impls);
         std::destroy_at(&_composite.dataMembs);
         std::destroy_at(&_composite.methods);
         std::destroy_at(&_composite.ops);
         std::destroy_at(&_composite.staticMembs);
         std::destroy_at(&_composite.staticFuncs);
         std::destroy_at(&_composite.subtypes);
         break;

      case FUNC_SIG:
         std::destroy_at(&_funcSig.params);
         break;
   }
}

clef::TypeSpec::TypeSpec(const TypeSpec& other):
   _metatype{other._metatype} {
      #define __cpy(field) __curr.field = other.__curr.field
      switch (_metatype) {
         case FUND_TYPE:
            #define __curr _fund
            __cpy(id);
            #undef __curr
            break;

         case INDIR:
            #define __curr _indir
            __cpy(pointee);
            __cpy(pointeeQuals);
            __cpy(table);
            #undef __curr
            break;

         case COMPOSITE:
            #define __curr _composite
            __cpy(tpltParams);
            __cpy(parentTypes);
            __cpy(impls);
            __cpy(dataMembs);
            __cpy(methods);
            __cpy(ops);
            __cpy(staticMembs);
            __cpy(staticFuncs);
            __cpy(subtypes);
            #undef __curr
            break;

         case FUNC_SIG:
            #define __curr _funcSig
            __cpy(retType);
            __cpy(params);
            #undef __curr
            break;
      }
      #undef __cpy
}

#endif //TYPE_SPEC_CPP