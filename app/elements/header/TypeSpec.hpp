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
         null,
         FUND_TYPE,
         INDIR,
         COMPOSITE,
         FUNC_SIG
      };
      struct ScopedSymbol {
         SymbolNode* symbol;
         QualMask quals;

         ScopedSymbol(): symbol{}, quals{} {}
         ScopedSymbol(SymbolNode* s): symbol{s}, quals{} {}
         ScopedSymbol(SymbolNode* s, QualMask q): symbol{s}, quals{q} {}

         SymbolNode& operator*() const { return *symbol; }
         SymbolNode* operator->() const { return symbol; }

         operator SymbolNode*() const { return symbol; }

         struct hash {
            using is_transparent = void;
            static ulong operator()(const ScopedSymbol& obj) { return std::hash<SymbolNode*>()(obj.symbol); }
         };
      };
   private:
      MetaType _metatype;
      SymbolNode* _canonName;
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
            mcsl::set<ScopedSymbol, ScopedSymbol::hash> impls; //implemented traits
            mcsl::dyn_arr<ScopedSymbol> dataMembs;
            mcsl::set<ScopedSymbol, ScopedSymbol::hash> methods;
            // mcsl::map<OpID, mcsl::map<mcsl::pair<TypeSpec*, TypeSpec*>, mcsl::pair<ScopedSymbol, index<void>>>> ops;
            mcsl::dyn_arr<ScopedSymbol> staticMembs;
            mcsl::set<ScopedSymbol, ScopedSymbol::hash> staticFuncs;
            mcsl::set<ScopedSymbol, ScopedSymbol::hash> subtypes;
         } _composite;
         struct { //FUNC_SIG
            TypeSpec* retType; //!retType -> auto
            mcsl::dyn_arr<mcsl::pair<TypeSpec*, QualMask>> params;
         } _funcSig;
      };

      friend class SyntaxTree;
   private:
      TypeSpec(TypeSpec* pointee, QualMask quals, IndirTable&& table):
         _metatype{INDIR},
         _indir{
            .pointee = pointee,
            .pointeeQuals = quals,
            .table = table
         } {

      }
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
         return TypeSpec{pointee, quals, firstEntry};
      }
      static TypeSpec makeIndir(TypeSpec* pointee, QualMask quals, IndirTable&& table) {
         return TypeSpec{pointee, quals, std::forward<IndirTable&&>(table)};
      }

      ~TypeSpec();

      MetaType metaType() const { return _metatype; }
      SymbolNode*& canonName() { return _canonName; }
      const SymbolNode* canonName() const { return _canonName; }

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


template<> struct std::hash<clef::TypeSpec::ScopedSymbol> {
   using is_transparent = void;
   clef::TypeSpec::ScopedSymbol::hash hash;
   auto operator()(const clef::TypeSpec::ScopedSymbol& obj) const { return hash(obj); }
};

/* |===============|
 * | SPECIAL CASES |
 * |===============|
 * _metatype == ENUM -> enumerators in _staticMembers
 * _metatype == MASK -> enumerators in _staticMembers
 * _metatype == UNION -> union members in _dataMembers
 */

#endif //TYPE_SPEC_HPP