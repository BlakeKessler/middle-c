#ifndef DATA_MODEL_CPP
#define DATA_MODEL_CPP

#include "DataModel.hpp"

clef::FundTypeID clef::toTypeID(KeywordID kw, DataModel model) {
   #define CASE(kw, typeid) [+KeywordID::kw] = FundTypeID::typeid
   static constexpr const FundTypeID bufLLP128[] = {
      CASE(VOID, VOID),
      CASE(AUTO, AUTO),


      CASE(CHAR, CHAR),
      CASE(CHAR_8, CHAR8),
      CASE(CHAR_16, CHAR16),
      CASE(CHAR_32, CHAR32),


      CASE(BOOL, BOOL),
      CASE(UBYTE, UINT8),
      CASE(USHORT, UINT16),
      CASE(UINT, UINT32),
      CASE(ULONG, UINT64),
      CASE(UOVERLONG, UINT128),
      CASE(UPTR, UINT64),
      CASE(UWORD, UINT64),

      CASE(UINT_8, UINT8),
      CASE(UINT_16, UINT16),
      CASE(UINT_32, UINT32),
      CASE(UINT_64, UINT64),
      CASE(UINT_128, UINT128),
      CASE(UINT_256, UINT256),


      CASE(SIGN_T, SIGN_T),
      CASE(SBYTE, SINT8),
      CASE(SSHORT, SINT16),
      CASE(SINT, SINT32),
      CASE(SLONG, SINT64),
      CASE(SOVERLONG, SINT128),
      CASE(SPTR, SINT64),
      CASE(SWORD, SINT64),

      CASE(SINT_8, SINT8),
      CASE(SINT_16, SINT16),
      CASE(SINT_32, SINT32),
      CASE(SINT_64, SINT64),
      CASE(SINT_128, SINT128),
      CASE(SINT_256, SINT256),

      CASE(FLOAT, FLOAT32),
      CASE(FLONG, FLOAT64),
      [+KeywordID::FLEXT] = (HAS_HW_FLOAT_80 ? FundTypeID::FLOAT80 : FundTypeID::FLOAT128),

      CASE(FLOAT_16, FLOAT16),
      CASE(FLOAT_32, FLOAT32),
      CASE(FLOAT_64, FLOAT64),
      CASE(FLOAT_80, FLOAT80),
      CASE(FLOAT_128, FLOAT128),
      CASE(FLOAT_256, FLOAT256),
   };
   static constexpr const FundTypeID bufLP64[] = {
      CASE(VOID, VOID),
      CASE(AUTO, AUTO),


      CASE(CHAR, CHAR),
      CASE(CHAR_8, CHAR8),
      CASE(CHAR_16, CHAR16),
      CASE(CHAR_32, CHAR32),


      CASE(BOOL, BOOL),
      CASE(UBYTE, UINT8),
      CASE(USHORT, UINT16),
      CASE(UINT, UINT32),
      CASE(ULONG, UINT64),
      CASE(UOVERLONG, UINT64),
      CASE(UPTR, UINT64),
      CASE(UWORD, UINT64),

      CASE(UINT_8, UINT8),
      CASE(UINT_16, UINT16),
      CASE(UINT_32, UINT32),
      CASE(UINT_64, UINT64),
      CASE(UINT_128, UINT128),
      CASE(UINT_256, UINT256),


      CASE(SIGN_T, SIGN_T),
      CASE(SBYTE, SINT8),
      CASE(SSHORT, SINT16),
      CASE(SINT, SINT32),
      CASE(SLONG, SINT64),
      CASE(SOVERLONG, SINT64),
      CASE(SPTR, SINT64),
      CASE(SWORD, SINT64),

      CASE(SINT_8, SINT8),
      CASE(SINT_16, SINT16),
      CASE(SINT_32, SINT32),
      CASE(SINT_64, SINT64),
      CASE(SINT_128, SINT128),
      CASE(SINT_256, SINT256),

      CASE(FLOAT, FLOAT32),
      CASE(FLONG, FLOAT64),
      [+KeywordID::FLEXT] = (HAS_HW_FLOAT_80 ? FundTypeID::FLOAT80 : FundTypeID::FLOAT128),

      CASE(FLOAT_16, FLOAT16),
      CASE(FLOAT_32, FLOAT32),
      CASE(FLOAT_64, FLOAT64),
      CASE(FLOAT_80, FLOAT80),
      CASE(FLOAT_128, FLOAT128),
      CASE(FLOAT_256, FLOAT256),
   };
   static constexpr const FundTypeID bufLP32[] = {
      CASE(VOID, VOID),
      CASE(AUTO, AUTO),


      CASE(CHAR, CHAR),
      CASE(CHAR_8, CHAR8),
      CASE(CHAR_16, CHAR16),
      CASE(CHAR_32, CHAR32),


      CASE(BOOL, BOOL),
      CASE(UBYTE, UINT8),
      CASE(USHORT, UINT16),
      CASE(UINT, UINT16),
      CASE(ULONG, UINT32),
      CASE(UOVERLONG, UINT64),
      CASE(UPTR, UINT32),
      CASE(UWORD, UINT32),

      CASE(UINT_8, UINT8),
      CASE(UINT_16, UINT16),
      CASE(UINT_32, UINT32),
      CASE(UINT_64, UINT64),
      CASE(UINT_128, UINT128),
      CASE(UINT_256, UINT256),


      CASE(SIGN_T, SIGN_T),
      CASE(SBYTE, SINT8),
      CASE(SSHORT, SINT16),
      CASE(SINT, SINT16),
      CASE(SLONG, SINT32),
      CASE(SOVERLONG, SINT64),
      CASE(SPTR, SINT32),
      CASE(SWORD, SINT32),

      CASE(SINT_8, SINT8),
      CASE(SINT_16, SINT16),
      CASE(SINT_32, SINT32),
      CASE(SINT_64, SINT64),
      CASE(SINT_128, SINT128),
      CASE(SINT_256, SINT256),

      CASE(FLOAT, FLOAT32),
      CASE(FLONG, FLOAT64),
      [+KeywordID::FLEXT] = (HAS_HW_FLOAT_80 ? FundTypeID::FLOAT80 : FundTypeID::FLOAT128),

      CASE(FLOAT_16, FLOAT16),
      CASE(FLOAT_32, FLOAT32),
      CASE(FLOAT_64, FLOAT64),
      CASE(FLOAT_80, FLOAT80),
      CASE(FLOAT_128, FLOAT128),
      CASE(FLOAT_256, FLOAT256),
   };
   static constexpr const FundTypeID bufILP32[] = {
      CASE(VOID, VOID),
      CASE(AUTO, AUTO),


      CASE(CHAR, CHAR),
      CASE(CHAR_8, CHAR8),
      CASE(CHAR_16, CHAR16),
      CASE(CHAR_32, CHAR32),


      CASE(BOOL, BOOL),
      CASE(UBYTE, UINT8),
      CASE(USHORT, UINT16),
      CASE(UINT, UINT32),
      CASE(ULONG, UINT32),
      CASE(UOVERLONG, UINT64),
      CASE(UPTR, UINT32),
      CASE(UWORD, UINT32),

      CASE(UINT_8, UINT8),
      CASE(UINT_16, UINT16),
      CASE(UINT_32, UINT32),
      CASE(UINT_64, UINT64),
      CASE(UINT_128, UINT128),
      CASE(UINT_256, UINT256),


      CASE(SIGN_T, SIGN_T),
      CASE(SBYTE, SINT8),
      CASE(SSHORT, SINT16),
      CASE(SINT, SINT32),
      CASE(SLONG, SINT32),
      CASE(SOVERLONG, SINT64),
      CASE(SPTR, SINT32),
      CASE(SWORD, SINT32),

      CASE(SINT_8, SINT8),
      CASE(SINT_16, SINT16),
      CASE(SINT_32, SINT32),
      CASE(SINT_64, SINT64),
      CASE(SINT_128, SINT128),
      CASE(SINT_256, SINT256),

      CASE(FLOAT, FLOAT32),
      CASE(FLONG, FLOAT64),
      [+KeywordID::FLEXT] = (HAS_HW_FLOAT_80 ? FundTypeID::FLOAT80 : FundTypeID::FLOAT128),

      CASE(FLOAT_16, FLOAT16),
      CASE(FLOAT_32, FLOAT32),
      CASE(FLOAT_64, FLOAT64),
      CASE(FLOAT_80, FLOAT80),
      CASE(FLOAT_128, FLOAT128),
      CASE(FLOAT_256, FLOAT256),
   };
   static constexpr const FundTypeID bufLLP64[] = {
      CASE(VOID, VOID),
      CASE(AUTO, AUTO),


      CASE(CHAR, CHAR),
      CASE(CHAR_8, CHAR8),
      CASE(CHAR_16, CHAR16),
      CASE(CHAR_32, CHAR32),


      CASE(BOOL, BOOL),
      CASE(UBYTE, UINT8),
      CASE(USHORT, UINT16),
      CASE(UINT, UINT32),
      CASE(ULONG, UINT32),
      CASE(UOVERLONG, UINT64),
      CASE(UPTR, UINT64),
      CASE(UWORD, UINT64),

      CASE(UINT_8, UINT8),
      CASE(UINT_16, UINT16),
      CASE(UINT_32, UINT32),
      CASE(UINT_64, UINT64),
      CASE(UINT_128, UINT128),
      CASE(UINT_256, UINT256),


      CASE(SIGN_T, SIGN_T),
      CASE(SBYTE, SINT8),
      CASE(SSHORT, SINT16),
      CASE(SINT, SINT32),
      CASE(SLONG, SINT32),
      CASE(SOVERLONG, SINT64),
      CASE(SPTR, SINT64),
      CASE(SWORD, SINT64),

      CASE(SINT_8, SINT8),
      CASE(SINT_16, SINT16),
      CASE(SINT_32, SINT32),
      CASE(SINT_64, SINT64),
      CASE(SINT_128, SINT128),
      CASE(SINT_256, SINT256),

      CASE(FLOAT, FLOAT32),
      CASE(FLONG, FLOAT64),
      [+KeywordID::FLEXT] = (HAS_HW_FLOAT_80 ? FundTypeID::FLOAT80 : FundTypeID::FLOAT128),

      CASE(FLOAT_16, FLOAT16),
      CASE(FLOAT_32, FLOAT32),
      CASE(FLOAT_64, FLOAT64),
      CASE(FLOAT_80, FLOAT80),
      CASE(FLOAT_128, FLOAT128),
      CASE(FLOAT_256, FLOAT256),
   };
   // static constexpr const FundTypeID bufILP64[] = {};
   // static constexpr const FundTypeID bufSILP64[] = {};
   static constexpr const FundTypeID bufx32[] = {
      CASE(VOID, VOID),
      CASE(AUTO, AUTO),


      CASE(CHAR, CHAR),
      CASE(CHAR_8, CHAR8),
      CASE(CHAR_16, CHAR16),
      CASE(CHAR_32, CHAR32),


      CASE(BOOL, BOOL),
      CASE(UBYTE, UINT8),
      CASE(USHORT, UINT16),
      CASE(UINT, UINT32),
      CASE(ULONG, UINT32),
      CASE(UOVERLONG, UINT64),
      CASE(UPTR, UINT32),
      CASE(UWORD, UINT64),

      CASE(UINT_8, UINT8),
      CASE(UINT_16, UINT16),
      CASE(UINT_32, UINT32),
      CASE(UINT_64, UINT64),
      CASE(UINT_128, UINT128),
      CASE(UINT_256, UINT256),


      CASE(SIGN_T, SIGN_T),
      CASE(SBYTE, SINT8),
      CASE(SSHORT, SINT16),
      CASE(SINT, SINT32),
      CASE(SLONG, SINT32),
      CASE(SOVERLONG, SINT128),
      CASE(SPTR, SINT32),
      CASE(SWORD, SINT64),

      CASE(SINT_8, SINT8),
      CASE(SINT_16, SINT16),
      CASE(SINT_32, SINT32),
      CASE(SINT_64, SINT64),
      CASE(SINT_128, SINT128),
      CASE(SINT_256, SINT256),

      CASE(FLOAT, FLOAT32),
      CASE(FLONG, FLOAT64),
      [+KeywordID::FLEXT] = (HAS_HW_FLOAT_80 ? FundTypeID::FLOAT80 : FundTypeID::FLOAT128),

      CASE(FLOAT_16, FLOAT16),
      CASE(FLOAT_32, FLOAT32),
      CASE(FLOAT_64, FLOAT64),
      CASE(FLOAT_80, FLOAT80),
      CASE(FLOAT_128, FLOAT128),
      CASE(FLOAT_256, FLOAT256),
   };

   static constexpr const FundTypeID* buf[] = {
      [(uint)DataModel::LLP128] = bufLLP128,
      [(uint)DataModel::LP64] = bufLP64,
      [(uint)DataModel::LP32] = bufLP32,
      [(uint)DataModel::ILP32] = bufILP32,
      [(uint)DataModel::LLP64] = bufLLP64,
      // [(uint)DataModel::ILP64] = bufILP64,
      // [(uint)DataModel::SILP64] = bufSILP64,
      [(uint)DataModel::x32] = bufx32,
   };
   return buf[(uint)model][+kw];
   #undef CASE
}

uint clef::sizeOf(FundTypeID id, DataModel model) {
   using enum FundTypeID;
   switch (model) {
      case DataModel::LLP128:
         debug_assert(id != __CHARS && id != __UINTS && id != __SINTS && id != __FLOATS && id != __INTS);
         if (id == VOID || id == AUTO || id == null) {
            return 0;
         }
         if (auto offset = +(id & ~__FLAGS) - 1; offset - 1) {
            return 1 << offset;
         } else {
            return 1;
         }
      
      case DataModel::LP64:
      case DataModel::LP32:
      case DataModel::ILP32:
      case DataModel::LLP64:
      case DataModel::ILP64:
      case DataModel::SILP64:
         TODO;
   }
}

#endif //DATA_MODEL_CPP