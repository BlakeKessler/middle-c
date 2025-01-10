#ifndef MCSL_ASSERT_HPP
#define MCSL_ASSERT_HPP

#include "MCSL.hpp"

#ifndef assert
   #define assert(expr) (static_cast<bool>(expr) ? void(0) : mcsl::__assert_fail(__FILE__, __LINE__, __func__, #expr))
#endif

#ifndef debug_assert
   #ifndef NDEBUG
      #define debug_assert(expr) assert(expr)
   #else
      #define debug_assert(expr) void(0)
   #endif
#endif

namespace mcsl {
   void __assert_fail(const char* file, const uint line, const char* func, const char* exprStr);
};

#include "../src/assert.cpp"

#endif //MCSL_ASSERT_HPP