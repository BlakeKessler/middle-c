#ifndef TYPE_DEF_HPP
#define TYPE_DEF_HPP

#include "CLEF.hpp"

#include "dyn_arr.hpp"

class clef::TypeDef {
   private:
      FundTypeID _metatype;
      union {
         struct { //object types //!TODO: maybe replace the dyn_arrs with sets
            mcsl::dyn_arr<SymbolNode*> _templateParams;
            mcsl::dyn_arr<SymbolNode*> _parentTypes;
            mcsl::dyn_arr<SymbolNode*> _impledInterfaces;
            mcsl::dyn_arr<SymbolNode*> _dataMembers;
            mcsl::dyn_arr<SymbolNode*> _methods;
            mcsl::dyn_arr<SymbolNode*> _staticMembers;
            mcsl::dyn_arr<SymbolNode*> _staticFuncs;
            mcsl::dyn_arr<SymbolNode*> _subtypes;
         };
         struct { //pointers and references
            SymbolNode* _pointedToType;
            QualMask _pttQuals;
            mcsl::dyn_arr<mcsl::pair<QualMask, FundTypeID>> _pointerTypes;
         };
      };

      struct PointerTypesHash {
         using is_transparent = void;
         static ulong operator()(mcsl::arr_span<mcsl::pair<QualMask, FundTypeID>> arr) {
            return std::hash<mcsl::str_slice>()(mcsl::str_slice{(char*)arr.begin(), (uint)(arr.size() * sizeof(arr[0]))});
            //!TODO: make sure padding bytes are always the same
            //!TODO: maybe use mcsl::arr_span<uint> instead of mcsl::str_slice
         }
         static ulong operator()(mcsl::dyn_arr<mcsl::pair<QualMask, FundTypeID>> arr) {
            return operator()(arr.span());
         }
      };

      friend class SyntaxTree;
   public:
      ~TypeDef() { TODO; }
};

/* |===============|
 * | SPECIAL CASES |
 * |===============|
 * _metatype == PTR -> pointer to _parentTypes[0]
 * _metatype == REF -> reference to _parentTypes[0]
 * _metatype == ENUM -> enumerators in _staticMembers
 * _metatype == MASK -> enumerators in _staticMembers
 * _metatype == UNION -> union members in _dataMembers
 */

//!TODO: maybe effectively make _parentTypes[0] in-place as an optimization?

#endif //TYPE_DEF_HPP