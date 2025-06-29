#ifndef AST_MANGLE_CPP
#define AST_MANGLE_CPP

#include "SyntaxTree.hpp"

//!ASSUMPTIONS:
// * non-global identifier
void clef::SyntaxTree::__mangleImpl(mcsl::File& file, clef::SyntaxTree& tree, clef::Identifier& name, clef::SyntaxTree::__MangleData& data) {
   //!TODO: qualifiers
   //name
   if (!name.scopeName()) {
      debug_assert(data.charsPrinted == 0);
      data.charsPrinted += file.printf(FMT(MANGLE_PREFIX "N%u%s"), name.name().size(), name.name());
   } else {
      //mangle parent scope
      __mangleImpl(file, tree, tree[name.scopeName()], data);

      //special cases for fundamental types
      if (name.fundTypeID() != FundTypeID::null) {
         TODO;
      }

      //print name itself
      sint subIndex = data.subIndex(name.symbol());
      if (subIndex == 0) {
         data.charsPrinted += file.printf(FMT("S_"));
      } else if(subIndex > 0) {
         data.charsPrinted += file.printf(FMT("S%,36u_"), subIndex - 1);
      } else {
         data.charsPrinted += file.printf(FMT("%u%s"), name.name().size(), name.name());
      }
   }

   //specializer
   if (name.specializer()) {
      __mangleSpecializerImpl(file, tree, name, data);
   }
   TODO;
}

#endif //AST_MANGLE_CPP