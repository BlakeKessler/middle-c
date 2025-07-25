#ifndef AST_MANGLE_CPP
#define AST_MANGLE_CPP

#include "SyntaxTree.hpp"
#include "pretty-print.hpp"

sint clef::SyntaxTree::__MangleData::subIndex(SymbolNode* symbol) {
   for (uint i = 0; i < substitutions.size(); ++i) {
      if (substitutions[i] == symbol) {
         return i;
      }
   }
   substitutions.push_back(symbol);
   return -1;
}

uint clef::SyntaxTree::manglePrint(mcsl::File& file, SyntaxTree& tree, SymbolNode* name) {
   __MangleData data;
   __mangleImpl(file, tree, name, data, 0);
   return data.charsPrinted;
}

void clef::SyntaxTree::__mangleImpl(mcsl::File& file, SyntaxTree& tree, SymbolNode* symbol, __MangleData& data, uint depth) {
   //fundamental type
   if (symbol->symbolType() == SymbolType::FUND_TYPE) {
      mcsl::writef(file, symbol->name(), 's', {});
      data.charsPrinted += symbol->name().size();
      return;
   }
   //indirect type
   else if (symbol->symbolType() == SymbolType::INDIR) {
      debug_assert(symbol->type()->pointee()->canonName());
      __mangleImpl(file, tree, symbol->type()->pointee()->canonName(), data, 0);
      file.write('*', symbol->type()->indirTable().size());
      data.charsPrinted += symbol->type()->indirTable().size();
      //!TODO: actually accurate implementation (based on the printf implementation for IndirTable)
      //!TODO: pointeeQuals
      return;
   }
   else if (symbol->symbolType() == SymbolType::GENERIC) {
      data.charsPrinted += file.printf(FMT("T_"));
      return;
      //!TODO: do this accurately
   }
   else if (symbol->symbolType() == SymbolType::EXTERN_TYPE) {
      TODO;
   }
   //composite type
   else if (isType(symbol->symbolType()) || symbol->symbolType() == SymbolType::VAR) {
      //name
      if (symbol->parentScope() == tree.globalScope()) {
         if (depth == 0) { //global symbols
            file.printf(symbol->name());
         } else { //global parent for non-global symbol
            // debug_assert(data.charsPrinted == 0);
            data.charsPrinted += file.printf(FMT(MANGLE_PREFIX MANGLE_NAMESPACE_OPEN "%u%s"), symbol->name().size(), symbol->name());
         }
      } else {
         //mangle parent scope
         __mangleImpl(file, tree, symbol->parentScope(), data, depth + 1);

         //print name itself
         sint subIndex = data.subIndex(symbol);
         if (subIndex == 0) {
            data.charsPrinted += file.printf(FMT("S_"));
         } else if(subIndex > 0) {
            data.charsPrinted += file.printf(FMT("S%,36u_"), subIndex - 1);
         } else {
            data.charsPrinted += file.printf(FMT("%u%s"), symbol->name().size(), symbol->name());
         }

         //namespace closer
         if (depth == 0) {
            file.write(FMT(MANGLE_NAMESPACE_CLOSE));
            data.charsPrinted++;
         }
      }

      // //specializer
      // if (name.specializer()) {
      //    __mangleSpecializerImpl(file, tree, name, data);
      // }
      // TODO;
      return;
   } else {
      TODO;
   }


   
}

#endif //AST_MANGLE_CPP