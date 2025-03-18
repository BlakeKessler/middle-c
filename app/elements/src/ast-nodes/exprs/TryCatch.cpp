#ifndef TRY_CATCH_CPP
#define TRY_CATCH_CPP

#include "ast-nodes/exprs/TryCatch.hpp"

#include "io.hpp"

void clef::TryCatch::printf() const {
   mcsl::printf(mcsl::FMT("TRY {id=%u} CATCH (id=%u) {id=%u}"), +procedure(), +err(), +errHandler());
}

#endif //TRY_CATCH_CPP