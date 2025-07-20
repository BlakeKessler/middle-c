#ifndef INDIR_TYPE_TABLE_HPP
#define INDIR_TYPE_TABLE_HPP

#include "CLEF.hpp"

#include "IndirTable.hpp"
#include "TypeSpec.hpp"
#include "hash.hpp"
#include "map.hpp"

class clef::IndirTypeTable {
   private:
      struct HashEntry {
         using is_transparent = void;
         HashEntry() = default;
         static inline uint64 operator()(const mcsl::pair<QualMask, IndirTable*> pair) {
            return mcsl::hash_algos::rapid_mix(pair.second->hash(), +pair.first);
         }
      };
      struct CmpEntry {
         using is_transparent = void;
         CmpEntry() = default;
         static inline bool operator()(const mcsl::pair<QualMask, IndirTable*> p1, const mcsl::pair<QualMask, IndirTable*> p2) {
            return p1.first == p2.first && *(p1.second) == *(p2.second);
         }
      };
      using SubTable = mcsl::map<mcsl::pair<QualMask, IndirTable*>, TypeSpec*, HashEntry, CmpEntry>;
      mcsl::map<TypeSpec*, SubTable> _table;
   public:
      IndirTypeTable():_table{} {}
      mcsl::pair<TypeSpec*, bool> get_or_insert(TypeSpec* pointee, QualMask pointeeQuals, IndirTable&& table, mcsl::arr_list<TypeSpec>& typeTable);
      inline void release() { _table.release(); }
};

#endif //INDIR_TYPE_TABLE_HPP