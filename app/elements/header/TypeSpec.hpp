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
         struct { //FUND_TYPE
            FundTypeID id;
         } _fund;
         struct { //INDIR
            TypeSpec* pointee;
            QualMask pointeeQuals;
            IndirTable table;
         } _indir;
         struct { //COMPOSITE
            mcsl::dyn_arr<SymbolNode*> tpltParams;
            mcsl::set<SymbolNode*> impls; //implemented interafaces
            mcsl::dyn_arr<SymbolNode*> dataMembs;
            mcsl::set<SymbolNode*> methods;
            mcsl::map<OpID, SymbolNode*> ops;
            mcsl::dyn_arr<SymbolNode*> staticMembs;
            mcsl::set<SymbolNode*> staticFuncs;
            mcsl::set<SymbolNode*> subtypes;
            //!TODO: SymbolNode* canonName;
         } _composite;
         struct { //FUNC_SIG
            TypeSpec* retType;
            mcsl::dyn_arr<TypeSpec*> params;
         } _funcSig;
      };

      friend class SyntaxTree;
   private:
      TypeSpec(TypeSpec* pointee, QualMask quals, IndirTable::Entry firstEntry):
         _metatype{INDIR},
         _indir{
            .pointee = pointee,
            .pointeeQuals = quals,
            .table = IndirTable{firstEntry}
         } {

      }
   public:
      TypeSpec(const TypeSpec&);
      TypeSpec(TypeSpec&&);
      TypeSpec(MetaType);
      TypeSpec(FundTypeID);
      static TypeSpec makeIndir(TypeSpec* pointee, QualMask quals, IndirTable::Entry firstEntry) {
         return TypeSpec{pointee,quals,firstEntry};
      }

      ~TypeSpec();

      MetaType metaType() const { return _metatype; }

      TypeSpec& operator=(const TypeSpec& other) { return *new (this) TypeSpec(other); }
      TypeSpec& operator=(TypeSpec&& other) { return *new (this) TypeSpec(std::move(other)); }

      IndirTable& indirTable() { assume(_metatype == INDIR); return _indir.table; }
      const IndirTable& indirTable() const { assume(_metatype == INDIR); return _indir.table; }
      TypeSpec* pointee() { assume(_metatype == INDIR); return _indir.pointee; }
      const TypeSpec* pointee() const { assume(_metatype == INDIR); return _indir.pointee; }
      QualMask& pointeeQuals() { assume(_metatype == INDIR); return _indir.pointeeQuals; }
      QualMask pointeeQuals() const { assume(_metatype == INDIR); return _indir.pointeeQuals; }


      auto& fund() { return _fund; }
      auto& composite() { return _composite; }
      auto& funcSig() { return _funcSig; }

      const auto& fund() const { return _fund; }
      const auto& composite() const { return _composite; }
      const auto& funcSig() const { return _funcSig; }

      bool operator==(const TypeSpec& other) const;
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