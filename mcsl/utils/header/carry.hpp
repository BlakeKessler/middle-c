#pragma once
#ifndef MCSL_CFARRY_HPP
#define MCSL_CFARRY_HPP

#include "MCSL.hpp"

namespace mcsl {
   //! +, return carry flag
   bool ADD_CF ( sint lhs,  sint rhs,  sint *res) { return __builtin_sadd_overflow(lhs, rhs, res); }
   bool ADD_CF (slong lhs, slong rhs, slong *res) { return __builtin_saddl_overflow(lhs, rhs, res); }
   bool ADD_CF ( uint lhs,  uint rhs,  uint *res) { return __builtin_uadd_overflow(lhs, rhs, res); }
   bool ADD_CF (ulong lhs, ulong rhs, ulong *res) { return __builtin_uaddl_overflow(lhs, rhs, res); }
   //! +=, return carry flag
   bool ADD_CF ( sint& lhs,  sint rhs) { return __builtin_sadd_overflow(lhs, rhs, &lhs); }
   bool ADD_CF (slong& lhs, slong rhs) { return __builtin_saddl_overflow(lhs, rhs, &lhs); }
   bool ADD_CF ( uint& lhs,  uint rhs) { return __builtin_uadd_overflow(lhs, rhs, &lhs); }
   bool ADD_CF (ulong& lhs, ulong rhs) { return __builtin_uaddl_overflow(lhs, rhs, &lhs); }


   //! -, return carry flag
   bool SUB_CF ( sint lhs,  sint rhs,  sint *res) { return __builtin_ssub_overflow(lhs, rhs, res); }
   bool SUB_CF (slong lhs, slong rhs, slong *res) { return __builtin_ssubl_overflow(lhs, rhs, res); }
   bool SUB_CF ( uint lhs,  uint rhs,  uint *res) { return __builtin_usub_overflow(lhs, rhs, res); }
   bool SUB_CF (ulong lhs, ulong rhs, ulong *res) { return __builtin_usubl_overflow(lhs, rhs, res); }
   //! -=, return carry flag
   bool SUB_CF ( sint& lhs,  sint rhs) { return __builtin_ssub_overflow(lhs, rhs, &lhs); }
   bool SUB_CF (slong& lhs, slong rhs) { return __builtin_ssubl_overflow(lhs, rhs, &lhs); }
   bool SUB_CF ( uint& lhs,  uint rhs) { return __builtin_usub_overflow(lhs, rhs, &lhs); }
   bool SUB_CF (ulong& lhs, ulong rhs) { return __builtin_usubl_overflow(lhs, rhs, &lhs); }


   //! *, return carry flag
   bool MUL_CF ( sint lhs,  sint rhs,  sint *res) { return __builtin_smul_overflow(lhs, rhs, res); }
   bool MUL_CF (slong lhs, slong rhs, slong *res) { return __builtin_smull_overflow(lhs, rhs, res); }
   bool MUL_CF ( uint lhs,  uint rhs,  uint *res) { return __builtin_umul_overflow(lhs, rhs, res); }
   bool MUL_CF (ulong lhs, ulong rhs, ulong *res) { return __builtin_umull_overflow(lhs, rhs, res); }
   //! *=, return carry flag
   bool MUL_CF ( sint& lhs,  sint rhs) { return __builtin_smul_overflow(lhs, rhs, &lhs); }
   bool MUL_CF (slong& lhs, slong rhs) { return __builtin_smull_overflow(lhs, rhs, &lhs); }
   bool MUL_CF ( uint& lhs,  uint rhs) { return __builtin_umul_overflow(lhs, rhs, &lhs); }
   bool MUL_CF (ulong& lhs, ulong rhs) { return __builtin_umull_overflow(lhs, rhs, &lhs); }
}

#endif //MCSL_CFARRY_HPP