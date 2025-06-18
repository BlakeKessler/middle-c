#ifndef TYPE_SPEC_HPP
#define TYPE_SPEC_HPP

#include "CLEF.hpp"
#include "IndirTable.hpp"

#include "dyn_arr.hpp"
#include "set.hpp"
#include "map.hpp"

class clef::TypeSpec {
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
            TypeSpec* pointee;
            QualMask pointeeQuals;
            IndirTable table;
         } _indir;
         struct {
            mcsl::set<TypeSpec*> tpltParams;
            mcsl::set<TypeSpec*> parentTypes;
            mcsl::set<TypeSpec*> impls; //implemented interafaces
            mcsl::set<TypeSpec*> dataMembs;
            mcsl::set<TypeSpec*> methods;
            mcsl::map<OpID, TypeSpec*> ops;
            mcsl::set<TypeSpec*> staticMembs;
            mcsl::set<TypeSpec*> staticFuncs;
            mcsl::set<TypeSpec*> subtypes;
         } _composite;
         struct {
            TypeSpec* retType;
            mcsl::dyn_arr<TypeSpec*> params;
         } _funcSig;
      };

      friend class SyntaxTree;
   private:
      TypeSpec(TypeSpec* pointee, QualMask quals, IndirTable::Entry firstEntry):
         _metatype{INDIR},
         _indir{
            pointee,
            quals,
            IndirTable{firstEntry}
         } {

      }
   public:
      TypeSpec(const TypeSpec&);
      TypeSpec(TypeSpec&&);
      static TypeSpec makeIndir(TypeSpec* pointee, QualMask quals, IndirTable::Entry firstEntry) {
         return TypeSpec{pointee,quals,firstEntry};
      }

      ~TypeSpec();

      TypeSpec& operator=(const TypeSpec& other) { return *new (this) TypeSpec(other); }
      TypeSpec& operator=(TypeSpec&& other) { return *new (this) TypeSpec(other); }

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

#endif //TYPE_SPEC_HPP