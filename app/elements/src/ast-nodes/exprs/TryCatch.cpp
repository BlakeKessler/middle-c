#ifndef TRY_CATCH_CPP
#define TRY_CATCH_CPP

#include "ast-nodes/exprs/TryCatch.hpp"

#include "io.hpp"

mcsl::File& mcsl::write(File& file, const clef::TryCatch& obj) {
   file.printf(FMT("TRY {id=%u} CATCH (id=%u) {id=%u}"), +obj.procedure(), +obj.err(), +obj.errHandler());
   return file;
}

#endif //TRY_CATCH_CPP