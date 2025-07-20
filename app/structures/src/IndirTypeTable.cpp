#ifndef INDIR_TYPE_TABLE_CPP
#define INDIR_TYPE_TABLE_CPP

#include "IndirTypeTable.hpp"

//returns a pair containing the type pointer and whether a new entry was added
mcsl::pair<clef::TypeSpec*, bool> clef::IndirTypeTable::get_or_insert(TypeSpec* pointee, QualMask pointeeQuals, IndirTable&& table, mcsl::arr_list<TypeSpec>& typeTable) {
   SubTable* ptr = _table.find(pointee);
   if (ptr) { //pointee type found
      TypeSpec** indirType = ptr->find(mcsl::pair{pointeeQuals, &table});
      if (indirType) { //type found
         std::destroy_at(&table);
         return {*indirType, false};
      }
   } else {
      ptr = &_table[pointee];
      new (ptr) SubTable();
   }

   //type not found - push type to tables
   TypeSpec* indirType = typeTable.push_back(TypeSpec::makeIndir(pointee, pointeeQuals, std::forward<IndirTable&&>(table)));
   (*ptr)[mcsl::pair{pointeeQuals, &indirType->indirTable()}] = indirType;
   return {indirType, true};
}

#endif //INDIR_TYPE_TABLE_CPP