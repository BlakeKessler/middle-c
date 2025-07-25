#ifndef CODE_GENERATOR_CPP
#define CODE_GENERATOR_CPP

#include "CodeGenerator.hpp"

void clef::CodeGenerator::gen(SyntaxTree& tree, mcsl::File& file) {
   CodeGenerator generator(tree, file);
   generator.initFile();
   generator.writeAllTypes();
   // generator.writeAllFuncs();

   // TODO;


}

void clef::CodeGenerator::writeDef(SymbolNode* symbol) {
   //print type
   SyntaxTree::manglePrint(out, tree, symbol->type()->canonName());
   out.write(' ');
   //print name
   out.write(symbol->name());
   // SyntaxTree::manglePrint(out, tree, symbol);
   out.write(';');
   out.write('\n');
}

void clef::CodeGenerator::writeAllTypes() {
   auto types = tree.allTypes();
   for (TypeSpec& spec : types) {
      //skip types that do not need to be manually defined
      if (spec.metaType() != TypeSpec::COMPOSITE) {
         continue;
      }
      auto& def = spec.composite();

      //print typedef
      if (spec.canonName()) {
         out.write(FMT("typedef "));
      }
      out.write(FMT("struct {\n"));
      for (auto& memb : def.dataMembs) {
         out.write(' ', 3);
         writeDef(memb.symbol);
      }
      out.write('}');
      if (spec.canonName()) {
         out.write(' ');
         SyntaxTree::manglePrint(out, tree, spec.canonName());
      }
      out.write(';');
      out.write('\n');

      //print static members
      for (auto& staticMemb : def.staticMembs) {
         writeDef(staticMemb.symbol);
      }
      // TODO;
   }
}

#endif