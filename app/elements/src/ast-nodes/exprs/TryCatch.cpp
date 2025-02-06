#ifndef TRY_CATCH_CPP
#define TRY_CATCH_CPP

#include "ast-nodes/exprs/TryCatch.hpp"

#include <cstdio>

void clef::TryCatch::printf() const {
   std::printf("TRY {id=%u} CATCH (id=%u) {id=%u}", +procedure(), +err(), +errHandler());
}

#endif //TRY_CATCH_CPP