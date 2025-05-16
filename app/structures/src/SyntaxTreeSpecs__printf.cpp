#ifndef SYNTAX_TREE_PRINTF_CPP
#define SYNTAX_TREE_PRINTF_CPP

#include "SyntaxTree.hpp"

#include "pretty-print.hpp"
#include "io.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

template<> void clef::SyntaxTree::__printf<clef::InterfaceSpec>(mcsl::File& file, index<const InterfaceSpec> i, const uint indents) const {
   TODO;
}
template<> void clef::SyntaxTree::__printf<clef::NamespaceSpec>(mcsl::File& file, index<const NamespaceSpec> i, const uint indents) const {
   TODO;
}
template<> void clef::SyntaxTree::__printf<clef::ObjTypeSpec>(mcsl::File& file, index<const ObjTypeSpec> i, const uint indents) const {
   TODO;
}

#pragma GCC diagnostic pop

#endif //SYNTAX_TREE_PRINTF_CPP