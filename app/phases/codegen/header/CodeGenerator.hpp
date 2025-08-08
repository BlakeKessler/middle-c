#ifndef CODE_GEN_HPP
#define CODE_GEN_HPP

#include "CLEF.hpp"
#include "SyntaxTree.hpp"
#include "io.hpp"
#include "set.hpp"

class clef::CodeGenerator {
   private:
      struct TpltInstance {
         private:
            SymbolNode* _symbol;
            uint64 _hash;
            mcsl::arr_span<index<Expr>> _tpltParams;

            static uint64 hashObj(SymbolNode*, mcsl::arr_span<index<Expr>>, SyntaxTree&);
            static uint64 hashExpr(SyntaxTree&, Expr&, uint64);
         public:
            TpltInstance(SymbolNode* symbol, mcsl::arr_span<index<Expr>> params, SyntaxTree& ast):
               _symbol(symbol),
               _hash(hashObj(symbol, params, ast)),
               _tpltParams(params) {

            }

            //!NOTE: subject to hash collisions
            bool operator==(const TpltInstance& other) const {
               return _hash == other._hash;
            }
      };

      mcsl::File& out;
      SyntaxTree& tree;

      mcsl::set<TpltInstance> instantcedTplts;

      uint indents;

      inline void nl() {
         out.write('\n');
         out.write(' ', indents * TAB_SIZE);
      }

      void initFile();

      void writeIden(index<Identifier>, bool);
      void writeTypeDef(index<TypeDecl>);

      void writeExpr(index<Expr>);
      void writeStmt(index<Stmt>);
      
      void writeStmtSeq(index<StmtSeq>);
      void writeProc(index<Scope>);

      void writeForwardDecl(index<astNode>);
      void writeDef(index<astNode>);
      void writeDef(SymbolNode*);
      
      CodeGenerator(SyntaxTree& t, mcsl::File& f): out{f}, tree{t}, instantcedTplts{}, indents{0} {}
   public:
      static void gen(SyntaxTree& t, mcsl::File& f);
      static void gen(SyntaxTree& t, mcsl::str_slice outPath) {
         mcsl::File f(outPath, "w");
         gen(t, f);
         f.close();
      }
};

#endif //CODE_GEN_HPP