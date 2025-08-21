#ifndef DATA_MODEL_HPP
#define DATA_MODEL_HPP

#include "CLEF.hpp"

namespace clef {
   //!TODO: separate the architecture from the data model (EX: x32 is actually an ABI to use ILP32 with the x86-64 instruction set)
   enum class DataModel {
      LP32,
      ILP32,
      LLP64,
      LP64,
      ILP64,
      SILP64,

      LLP128,

      x32,
   };
   constexpr bool HAS_HW_FLOAT_80 = true;
   FundTypeID toTypeID(KeywordID, DataModel);
   uint sizeOf(FundTypeID, DataModel);
   inline uint sizeOf(KeywordID id, DataModel m) { return sizeOf(toTypeID(id, m), m); }

};

#endif //DATA_MODEL_HPP