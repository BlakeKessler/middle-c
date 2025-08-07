#ifndef CODE_GENERATOR_CPP
#define CODE_GENERATOR_CPP

#include "CodeGenerator.hpp"

#include "pretty-print.hpp"
#include "hash.hpp"

uint64 clef::CodeGenerator::TpltInstance::hashObj(SymbolNode* symbol, mcsl::arr_span<index<Expr>> params, SyntaxTree& tree) {
   uint64 val = mcsl::hash_algos::rapid_mix((uint64)symbol, 0);
   for (index<Expr> i : params) {
      debug_assert(i);
      val = hashExpr(tree, tree[i], val);
   }
   return val;
}
uint64 clef::CodeGenerator::TpltInstance::hashExpr(SyntaxTree& tree, Expr& expr, uint64 val) {
   val = mcsl::hash_algos::rapid_mix(val, +expr.opID());
   #define HASH(operand, n) \
      if (expr.operand##n()) {                                               \
         if (canDownCastTo(expr.operand##Type##n(), NodeType::EXPR)) {       \
            val = hashExpr(tree, tree[(index<Expr>)expr.operand##n()], val); \
         } else {                                                            \
            TODO;                                                            \
         }                                                                   \
      }
   HASH(lhs,);
   HASH(rhs,);
   HASH(extra,);
   HASH(extra, 2);
   #undef HASH

   return val;
}

void clef::CodeGenerator::gen(SyntaxTree& tree, mcsl::File& file) {
   CodeGenerator generator(tree, file);
   generator.initFile();

   debug_assert(tree[(index<astNode>)1].nodeType() == NodeType::STMT_SEQ);
   generator.writeStmtSeq(1);
}

void clef::CodeGenerator::writeIden(index<Identifier> i, bool doMangle) {
   debug_assert(i);
   
   if (doMangle) {
      SyntaxTree::manglePrint(out, tree, i);
   }
   else {
      Identifier& iden = tree[i];
      if (iden.specializer()) {
         TODO;
      }
      out.write(iden.name());
   }
}

void clef::CodeGenerator::writeStmt(index<Stmt> stmt) {
   writeExpr(stmt);
   out.write(';');
   out.write('\n');
}
void clef::CodeGenerator::writeExpr(index<Expr> i) {
   debug_assert(i);
   Expr& expr = tree[i];

   using enum OpID;
   switch (expr.opID()) {
      //keyword pseudo-operators
      case FOR      : {
         ForLoop& forLoop = tree[(index<ForLoop>)i];
         //for
         out.write(FMT("for ("));
         //for loop parameters
         writeExpr(forLoop.decl());
         out.write(';');
         writeExpr(forLoop.condition()); //!TODO: cast to bool
         out.write(';');
         writeExpr(forLoop.increment());
         out.write(FMT(")"));
         //procedure
         writeProc(+forLoop.procedure());
      } break;
      case FOREACH  : {
         //!NOTE: maybe desugar foreach loops during parsing?
         ForeachLoop& foreachLoop = tree[(index<ForeachLoop>)i];
         TODO;
      } break;
      case WHILE    : {
         WhileLoop& whileLoop = tree[(index<WhileLoop>)i];
         //while
         out.write(FMT("while ("));
         //condition
         writeExpr(whileLoop.condition()); //!TODO: cast to bool
         out.write(FMT(")"));
         //procedure
         writeProc(+whileLoop.procedure());
      } break;
      case DO_WHILE : {
         DoWhileLoop& whileLoop = tree[(index<DoWhileLoop>)i];
         //while
         out.write(FMT("do "));
         //procedure
         writeProc(+whileLoop.procedure());
         //condition
         out.write(FMT(" while ("));
         writeExpr(whileLoop.condition()); //!TODO: cast to bool
         out.write(FMT(")"));
      } break;
      case IF       : {
         If& whileLoop = tree[(index<If>)i];
         //while
         out.write(FMT("if ("));
         //condition
         writeExpr(whileLoop.condition()); //!TODO: cast to bool
         out.write(FMT(")"));
         //procedure
         writeProc(+whileLoop.procedure());
      } break;
      case SWITCH   : {
         TODO;
      } break;
      case MATCH    : {
         TODO;
      } break;
      case ASM      : {
         TODO;
      } break;

      case GOTO: {
         if (!canDownCastTo(expr.lhsType(), NodeType::IDEN)) {
            TODO;
         }
         //!NOTE: intentionally does not use `writeIden`
         out.write(FMT("goto "));
         out.write(tree[(index<Identifier>)expr.lhs()].name());
      } break;
      case GOTO_CASE: {
         TODO;
      } break;

      case LET      : {
         //!NOTE: if `typedef struct` and `typedef union` are not used when MAKE_TYPE is implemented, `struct`/`union` will need to be printed for object types
         Decl& decl = tree[(index<Decl>)i];
         //type
         writeIden(decl.type(), true);
         out.write(' ');
         //name
         writeIden(decl.name(), false);
         //definition
         if (decl.value()) {
            out.write(FMT(" = "));
            writeExpr(decl.value());
         }
      } break;
      case MAKE_TYPE: {
         TODO;
      } break;

      case RETURN   : {
         out.write(FMT("return "));
         writeExpr(+expr.lhs());
      } break;
      case BREAK    : {
         out.write(FMT("break"));
      } break;
      case CONTINUE : {
         out.write(FMT("continue"));
      } break;

      #pragma region normalops
      case INC: [[fallthrough]];
      case DEC: [[fallthrough]];
      case BIT_NOT: [[fallthrough]];
      
      case BIT_AND: [[fallthrough]];
      case BIT_OR: [[fallthrough]];
      case BIT_XOR: [[fallthrough]];
      case SHIFT_LEFT: [[fallthrough]];
      case SHIFT_RIGHT: [[fallthrough]];
      
      case ADD: [[fallthrough]];
      case SUB: [[fallthrough]];
      case MUL: [[fallthrough]];
      case DIV: [[fallthrough]];
      case MOD: [[fallthrough]];
      case EXP: [[fallthrough]];
      case COALESCE: [[fallthrough]];

      case LOGICAL_NOT: [[fallthrough]];
      case LOGICAL_AND: [[fallthrough]];
      case LOGICAL_OR: [[fallthrough]];
      case LESSER: [[fallthrough]];
      case GREATER: [[fallthrough]];
      case LESSER_OR_EQ: [[fallthrough]];
      case GREATER_OR_EQ: [[fallthrough]];
      case IS_EQUAL: [[fallthrough]];
      case IS_UNEQUAL: [[fallthrough]];
      case THREE_WAY_COMP: [[fallthrough]];

      case ASSIGN: [[fallthrough]];
      case ADD_ASSIGN: [[fallthrough]];
      case SUB_ASSIGN: [[fallthrough]];
      case MUL_ASSIGN: [[fallthrough]];
      case DIV_ASSIGN: [[fallthrough]];
      case MOD_ASSIGN: [[fallthrough]];
      case EXP_ASSIGN: [[fallthrough]];
      case SHL_ASSIGN: [[fallthrough]];
      case SHR_ASSIGN: [[fallthrough]];
      case AND_ASSIGN: [[fallthrough]];
      case XOR_ASSIGN: [[fallthrough]];
      case OR_ASSIGN: [[fallthrough]];
      case COALESCE_ASSIGN: {
         debug_assert(!expr.extra() && !expr.extra2());
         debug_assert(expr.lhs() || expr.rhs());
         TypeSpec* __tmp;
         #define IS_PRIM(name) (!expr.name() || !(__tmp = tree.evalType(+expr.name())) || __tmp->metaType() == TypeSpec::FUND_TYPE || (__tmp->metaType() == TypeSpec::INDIR && __tmp->indirTable().back().decaysToFund()))
         bool isAllPrimitive = IS_PRIM(lhs) && IS_PRIM(rhs);
         #undef IS_PRIM

         if (isAllPrimitive) {
            //!NOTE: ignores precedence and associativity, always uses parens
            //opening paren
            out.write('(');
            //lhs
            if (expr.lhs()) {
               writeExpr(+expr.lhs());
            }
            //operator
            out.write(toString(expr.opID()));
            //rhs
            if (expr.rhs()) {
               writeExpr(+expr.rhs());
            }
            //closing paren
            out.write(')');
         } else {
            TODO;
         }
      } break;
      #pragma endregion normalops

      case CALL_INVOKE: [[fallthrough]];
      case SUBSCRIPT_INVOKE: TODO;

      case COMMA: TODO;

      case NULL:
         if ((bool)(expr.lhs()) != (bool)(expr.rhs())) {
            TODO;
         } else {
            if (expr.lhs() || expr.rhs() || expr.extra() || expr.extra2()) {
               TODO;
            } else {
               TODO;
            }
         }

      case ALIAS:
         return; //!NOTE: not sure that this works

      default:
         internalError(ErrCode::BAD_EXPR, FMT("unhandled operator `%s` reached"), toString(expr.opID()));
   }
}

void clef::CodeGenerator::writeProc(index<Scope> i) {
   debug_assert(i);

   out.write(FMT(" {"));
   ++indents;
   nl();
   writeStmtSeq(i);
   --indents;
   nl();
   out.write('}');
}
void clef::CodeGenerator::writeStmtSeq(index<StmtSeq> i) {
   debug_assert(i);

   StmtSeq& stmts = tree[i];
   auto span = stmts.span();
   for (index<Stmt> stmt : span) {
      writeStmt(stmt);
   }
}

#endif