#ifndef CODE_GEN_HPP
#define CODE_GEN_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"
#include "io.hpp"

class clef::CodeGenerator {
   private:
      mcsl::File& out;
      SyntaxTree& tree;

      void initFile();

      void writeAllTypes();
      void writeAllFuncs();

      void writeExpr(index<Expr>);
      void writeStmt(index<Stmt>);
      void writeStmtSeq(index<astNode>);
      void writeForwardDecl(index<astNode>);
      void writeDef(index<astNode>);
      void writeDef(SymbolNode*);
      
      CodeGenerator(SyntaxTree& t, mcsl::File& f): out{f}, tree{t} {}
   public:
      static void gen(SyntaxTree& t, mcsl::File& f);
      static void gen(SyntaxTree& t, mcsl::str_slice outPath) {
         mcsl::File f(outPath, "w");
         gen(t, f);
         f.close();
      }
};

#endif //CODE_GEN_HPP