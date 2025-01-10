#ifndef MCSL_ASSERT_CPP
#define MCSL_ASSERT_CPP

#include "assert.hpp"
#include "mcsl_throw.hpp"

void mcsl::__assert_fail(const char* file, const uint line, const char* func, const char* exprStr) {
   mcsl::mcsl_throw(ErrCode::ASSERT_FAIL, "%s:%u: %s: Assertion `%s` failed.", file, line, func, exprStr);
}

#endif //MCSL_ASSERT_CPP