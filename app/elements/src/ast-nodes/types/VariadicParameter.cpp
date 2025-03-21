#ifndef VAR_PARAM_CPP
#define VAR_PARAM_CPP

#include "ast-nodes/types/VariadicParameter.hpp"

mcsl::File& mcsl::write(File& file, const clef::VariadicParam& obj) {
   obj.printAs(file, FMT("variadic parameter"));
   return file;
}

#endif //VAR_PARAM_CPP