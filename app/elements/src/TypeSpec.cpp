#ifndef TYPE_SPEC_CPP
#define TYPE_SPEC_CPP

#include "TypeSpec.hpp"
#include "SymbolNode.hpp"

clef::TypeSpec::TypeSpec(MetaType metatype):_metatype{metatype} {
   switch (metatype) {
      case FUND_TYPE:
         _fund = decltype(_fund){};
         break;
      case INDIR:
         UNREACHABLE;
         break;
      case COMPOSITE:
         _composite = decltype(_composite){};
         break;
      case FUNC_SIG:
         _funcSig = decltype(_funcSig){};
         break;
   }
}
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

bool clef::TypeSpec::operator==(const TypeSpec& other) const {
   if (this == &other) {
      return true;
   }
   switch (_metatype) {
      case FUND_TYPE:
         return _fund.id == other._fund.id;
      case INDIR:
         TODO;
      case COMPOSITE:
         #define __checkSize(entry) if (_composite.entry.size() != other._composite.entry.size()) { return false; }
         __checkSize(tpltParams);
         __checkSize(impls);
         __checkSize(dataMembs);
         __checkSize(methods);
         __checkSize(ops);
         __checkSize(staticMembs);
         __checkSize(staticFuncs);
         __checkSize(subtypes);
         #undef __checkSize

         #define __checkArr(field) \
            for (uint i = 0; i < _composite.field.size(); ++i) { \
               if (_composite.field[i] != other._composite.field[i] && _composite.field[i]->type() != other._composite.field[i]->type() && *_composite.field[i]->type() != *other._composite.field[i]->type()) { \
                  return false; \
               } \
            }
         __checkArr(tpltParams);
         __checkArr(dataMembs);
         __checkArr(staticMembs);
         #undef __checkArr

         #define __checkSet(field) \
            for (SymbolNode* node : _composite.field) { \
               if (!other._composite.field.contains(node)) { \
                  return false; \
               } \
            }
         __checkSet(impls);
         __checkSet(methods);
         __checkSet(staticFuncs);
         __checkSet(subtypes);
         #undef __checkSet
         
         #define __checkMap(field) \
            for (auto [op, def] : _composite.field) { \
               if (!other._composite.field.contains(op) || other._composite.field[op] != def) { \
                  return false; \
               } \
            }
         __checkMap(ops);
         #undef __checkMap

         return true;
      case FUNC_SIG:
         if (_funcSig.retType != other._funcSig.retType) {
            return false;
         }
         if (_funcSig.params.size() != other._funcSig.params.size()) {
            return false;
         }
         for (uint i = 0; i < _funcSig.params.size(); ++i) {
            if (*_funcSig.params[i] != *other._funcSig.params[i]) {
               return false;
            }
         }
         return true;
   }
}

#endif //TYPE_SPEC_CPP