#ifndef TYPE_DEF_HPP
#define TYPE_DEF_HPP

#include "CLEF.hpp"
#include "IndirTable.hpp"

#include "dyn_arr.hpp"
#include "set.hpp"
#include "map.hpp"

class clef::TypeDef {
   public:
      enum MetaType : uint8 {
         FUND_TYPE,
         INDIR,
         COMPOSITE,
         FUNC_SIG
      };
   private:
      MetaType _metatype;
      union {
         struct {
            FundTypeID id;
         } _fund;
         struct {
            TypeDef* pointee;
            QualMask pointeeQuals;
            IndirTable table;
         } _indir;
         struct {
            mcsl::set<TypeDef*> tpltParams;
            mcsl::set<TypeDef*> parentTypes;
            mcsl::set<TypeDef*> impls; //implemented interafaces
            mcsl::set<TypeDef*> dataMembs;
            mcsl::set<TypeDef*> methods;
            mcsl::map<OpID, TypeDef*> ops;
            mcsl::set<TypeDef*> staticMembs;
            mcsl::set<TypeDef*> staticFuncs;
            mcsl::set<TypeDef*> subtypes;
         } _composite;
         struct {
            TypeDef* retType;
            mcsl::dyn_arr<TypeDef*> params;
         } _funcSig;
      };

      friend class SyntaxTree;
   public:
      TypeDef(MetaType);
      TypeDef(FundTypeID);
      ~TypeDef() { TODO; }

      IndirTable& indirTable() { assume(_metatype == INDIR); return _indir.table; }
      const IndirTable& indirTable() const { assume(_metatype == INDIR); return _indir.table; }
};

/* |===============|
 * | SPECIAL CASES |
 * |===============|
 * _metatype == ENUM -> enumerators in _staticMembers
 * _metatype == MASK -> enumerators in _staticMembers
 * _metatype == UNION -> union members in _dataMembers
 */

//!TODO: maybe effectively make _parentTypes[0] in-place as an optimization?

#endif //TYPE_DEF_HPP