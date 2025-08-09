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

   generator.nl();
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
void clef::CodeGenerator::writeTypename(TypeSpec* spec) {
   debug_assert(spec);

   const mcsl::str_slice cMetaType = [](SymbolType t) -> const mcsl::str_slice {
      using enum SymbolType;
      switch (t) {
         case STRUCT: fthru;
         case CLASS: fthru;
         case ENUM_UNION: return FMT("struct");

         case UNION: return FMT("union");
         
         case MASK: fthru;
         case ENUM: return FMT("enum");

         case TRAIT: return {(char*)(-1), ~0u};
         case NAMESPACE: return {};

         case GENERIC: TODO;
         case EXTERN_TYPE: TODO;

         case FUND_TYPE: fthru;
         case INDIR: return {};

         case VAR: fthru;
         case FUNC: fthru;
         case MACRO: fthru;
         case ATTRIBUTE: fthru;
         case LABEL: fthru;
         case EXTERN_IDEN: fthru;
         case null: UNREACHABLE;
         case __TYPE_BIT: UNREACHABLE;
      }
   }(spec->canonName()->symbolType());

   if (cMetaType.data() && ~(uptr)cMetaType.data()) {
      out.writeln(cMetaType, ' ');
   }

   //!TODO: specializer
   SyntaxTree::manglePrint(out, tree, spec->canonName(), {});
}

void clef::CodeGenerator::writeTypeDef(SymbolNode* symbol) {
   debug_assert(symbol);
   TypeSpec* spec = symbol->type();
   debug_assert(spec);

   if (spec->metaType() != TypeSpec::COMPOSITE) {
      TODO;
   }

   bool isObjType;
   bool isEnumLike;
   switch (symbol->symbolType()) {
      using enum SymbolType;
      #include "MAP_MACRO.h"

      CASES(MASK, ENUM):
         isObjType = false;
         isEnumLike = true;
         break;
      CASES(STRUCT, CLASS, ENUM_UNION, UNION):
         isObjType = true;
         isEnumLike = false;
         break;

      case NAMESPACE:
         isObjType = false;
         isEnumLike = false;
         break;
      
      case TRAIT: return; //traits only exist at compile-time, so do not need to be represented in the executable

      case GENERIC: TODO;
      case EXTERN_TYPE: TODO;

      CASES(FUND_TYPE, INDIR): 
         UNREACHABLE;

      CASES(VAR, FUNC, MACRO, ATTRIBUTE, LABEL, EXTERN_IDEN, null):
         UNREACHABLE;
      case __TYPE_BIT:
         UNREACHABLE;

      #include "MAP_MACRO_UNDEF.h"
   }

   //!forward-declare type
   if (isObjType) {
      writeTypename(symbol->type()); //!TODO: specializer
      out.write(';');
      nl();
   }

   //!define subtypes
   for (auto subtype : spec->composite().subtypes) {
      writeTypeDef(subtype.symbol);
      nl();
   }

   //!struct for type (member names not mangled)
   if (isObjType) {
      writeTypename(symbol->type()); //!TODO: specializer
      out.write(FMT(" {"));

      auto span = spec->composite().dataMembs.span();
      if (span.size()) {
         ++indents;
         for (auto elem : span) {
            nl();
            writeTypename(elem.symbol->type());
            out.write(' ');
            out.write(elem.symbol->name());
            //!TODO: value
            out.write(';');
         }
         --indents;
         nl();
      }
      
      out.write(FMT("};"));
      nl();
   }

   //!static members
   if (isEnumLike) {
      auto span = spec->composite().staticMembs.span();
      writeTypename(symbol->type()); //!TODO: specializer
      out.write(FMT(" {"));
      if (span.size()) {
         ++indents;
         for (auto elem : span) {
            nl();
            out.write(elem.symbol->name());
            //!TODO: value
            out.write(',');
         }
         --indents;
         nl();
      }
      out.write(FMT("};"));
   } else {
      auto span = spec->composite().staticMembs.span();
      for (auto elem : span) {
         writeTypename(elem.symbol->type());
         out.write(' ');
         SyntaxTree::manglePrint(out, tree, elem.symbol, {});
         //!TODO: value
         out.write(';');
         nl();
      }
   }

   //!forward-declare functions and methods
   for (auto elem : spec->composite().methods) {
      auto overloads = elem.symbol->overloads().span();
      for (auto [sigType, defIndex] : overloads) {
         debug_assert(sigType->metaType() == TypeSpec::FUNC_SIG);
         auto& sig = sigType->funcSig();
         writeTypename(sig.retType);
         out.write(' ');
         SyntaxTree::manglePrint(out, tree, elem.symbol, {}, sigType); //!TODO: specializer
         out.write('(');

         writeTypename(sig.selfType);
         out.write('*');

         for (auto [paramSpec, paramQuals] : sig.params.span()) {
            out.write(',').write(' ');
            //!TODO: quals
            writeTypename(paramSpec);
         }

         out.write(')').write(';');
         nl();
      }
   }

   for (auto elem : spec->composite().staticFuncs) {
      auto overloads = elem.symbol->overloads().span();
      for (auto [sigType, defIndex] : overloads) {
         debug_assert(sigType->metaType() == TypeSpec::FUNC_SIG);
         auto& sig = sigType->funcSig();
         writeTypename(sig.retType);
         out.write(' ');
         SyntaxTree::manglePrint(out, tree, elem.symbol, {}, sigType); //!TODO: specializer
         out.write('(');

         if (sig.params.size()) {
            //!TODO: quals
            writeTypename(sig.params[0].first);
            for (auto [paramSpec, paramQuals] : sig.params.span(1, sig.params.size())) {
               out.write(',').write(' ');
               //!TODO: quals
               writeTypename(paramSpec);
            }
         }

         out.write(')').write(';');
         nl();
      }
   }

   //!define functions and methods (parameter names not mangled)
   for (auto elem : spec->composite().methods) {
      auto overloads = elem.symbol->overloads().span();
      for (auto [sigType, defIndex] : overloads) {
         if (!defIndex) { //function is only forward-declared
            continue;
         }

         debug_assert(sigType->metaType() == TypeSpec::FUNC_SIG);
         auto& sig = sigType->funcSig();
         writeTypename(sig.retType);
         out.write(' ');
         SyntaxTree::manglePrint(out, tree, elem.symbol, {}, sigType); //!TODO: specializer
         out.write('(');

         writeTypename(sig.selfType);
         out.write(FMT("* this"));

         FuncDef def = tree[defIndex];
         auto params = tree[def.params()].span();
         for (index<Expr> exprIndex : params) {
            out.write(',').write(' ');
            Expr& expr = tree[exprIndex];
            debug_assert(expr.opID() == OpID::LET);
            Decl& decl = tree[(index<Decl>)exprIndex];

            writeTypename(tree[decl.type()].symbol()->type()); //!TODO: specializer
            // writeIden(decl.type(), true);
            out.write(' ');
            out.write(tree[decl.name()].symbol()->name());
            //!TODO: default value
         }

         out.write(FMT(") "));
         writeProc(def.procedure());
         nl();
      }
   }

   for (auto elem : spec->composite().staticFuncs) {
      auto overloads = elem.symbol->overloads().span();
      for (auto [sigType, defIndex] : overloads) {
         if (!defIndex) { //function is only forward-declared
            continue;
         }

         debug_assert(sigType->metaType() == TypeSpec::FUNC_SIG);
         auto& sig = sigType->funcSig();
         writeTypename(sig.retType);
         out.write(' ');
         SyntaxTree::manglePrint(out, tree, elem.symbol, {}, sigType); //!TODO: specializer
         out.write('(');

         debug_assert(!sig.selfType);

         FuncDef def = tree[defIndex];
         auto params = tree[def.params()].span();
         {
            Expr& expr = tree[params[0]];
            debug_assert(expr.opID() == OpID::LET);
            Decl& decl = tree[(index<Decl>)params[0]];

            writeTypename(tree[decl.type()].symbol()->type()); //!TODO: specializer
            // writeIden(decl.type(), true);
            out.write(' ');
            out.write(tree[decl.name()].symbol()->name());
            //!TODO: default value
         }
         for (index<Expr> exprIndex : params.span(1, params.size())) {
            out.write(',').write(' ');
            Expr& expr = tree[exprIndex];
            debug_assert(expr.opID() == OpID::LET);
            Decl& decl = tree[(index<Decl>)exprIndex];

            writeTypename(tree[decl.type()].symbol()->type()); //!TODO: specializer
            // writeIden(decl.type(), true);
            out.write(' ');
            out.write(tree[decl.name()].symbol()->name());
            //!TODO: default value
         }

         out.write(FMT(") "));
         writeProc(def.procedure());
         nl();
      }
   }
}

void clef::CodeGenerator::writeStmt(index<Stmt> stmt) {
   nl();
   writeExpr(stmt);
   out.write(';');
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
         out.write(FMT(") "));
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
         out.write(FMT(") "));
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
         out.write(')');
      } break;
      case IF       : { //!TODO: else
         If& ifStmt = tree[(index<If>)i];
         if (ifStmt.condition()) {
            //if
            out.write(FMT("if ("));
            //condition
            writeExpr(ifStmt.condition()); //!TODO: cast to bool
            out.write(FMT(") "));
         }
         //procedure
         writeProc(+ifStmt.procedure());
         //else
         if (ifStmt.elseStmt()) {
            nl();
            out.write(FMT("else "));
            writeExpr(ifStmt.elseStmt());
         }

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
         writeTypename(tree[decl.type()].symbol()->type());
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
         TypeDecl& decl = tree[(index<TypeDecl>)i];
         if (!decl.decl()) {
            TODO;
         }
         writeTypeDef(tree[decl.decl()].symbol());
      } break;

      case RETURN   : {
         out.write(FMT("return "));
         writeValue(+expr.lhs());
      } break;
      case BREAK    : {
         out.write(FMT("break"));
      } break;
      case CONTINUE : {
         out.write(FMT("continue"));
      } break;

      #pragma region normalops
      case INC: fthru;
      case DEC: fthru;
      case BIT_NOT: fthru;
      
      case BIT_AND: fthru;
      case BIT_OR: fthru;
      case BIT_XOR: fthru;
      case SHIFT_LEFT: fthru;
      case SHIFT_RIGHT: fthru;
      
      case ADD: fthru;
      case SUB: fthru;
      case MUL: fthru;
      case DIV: fthru;
      case MOD: fthru;
      case EXP: fthru;
      case COALESCE: fthru;

      case LOGICAL_NOT: fthru;
      case LOGICAL_AND: fthru;
      case LOGICAL_OR: fthru;
      case LESSER: fthru;
      case GREATER: fthru;
      case LESSER_OR_EQ: fthru;
      case GREATER_OR_EQ: fthru;
      case IS_EQUAL: fthru;
      case IS_UNEQUAL: fthru;
      case THREE_WAY_COMP: fthru;

      case ASSIGN: fthru;
      case ADD_ASSIGN: fthru;
      case SUB_ASSIGN: fthru;
      case MUL_ASSIGN: fthru;
      case DIV_ASSIGN: fthru;
      case MOD_ASSIGN: fthru;
      case EXP_ASSIGN: fthru;
      case SHL_ASSIGN: fthru;
      case SHR_ASSIGN: fthru;
      case AND_ASSIGN: fthru;
      case XOR_ASSIGN: fthru;
      case OR_ASSIGN: fthru;
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
               writeValue(+expr.lhs());
            }
            //operator
            out.write(toString(expr.opID()));
            //rhs
            if (expr.rhs()) {
               writeValue(+expr.rhs());
            }
            //closing paren
            out.write(')');
         } else {
            TODO;
         }
      } break;
      #pragma endregion normalops

      case CALL_INVOKE: fthru;
      case SUBSCRIPT_INVOKE: TODO;

      case COMMA: TODO;

      case NULL:
         if ((bool)(expr.lhs()) != (bool)(expr.rhs())) {
            if (expr.lhs()) {
               writeValue(+expr.lhs());
            } else {
               writeValue(+expr.rhs());
            }
         } else {
            if (expr.lhs() || expr.rhs() || expr.extra() || expr.extra2()) {
               TODO;
            } else {
               TODO;
            }
         }
         break;

      case DEF_FUNC_PARAMS: fthru;
      case ALIAS:
         return; //!NOTE: not sure that this works

      case MEMBER_ACCESS:
         writeValue(+expr.lhs());
         out.write('.');
         writeValue(+expr.rhs());
         break;

      default:
         internalError(ErrCode::BAD_EXPR, FMT("unhandled operator `%s` reached"), toString(expr.opID()));
   }
}

void clef::CodeGenerator::writeValue(index<astNode> i) {
   debug_assert(i);
   astNode& node = tree[i];
   if (canDownCastTo(node.nodeType(), NodeType::EXPR)) {
      writeExpr(+i);
      return;
   } else if (canDownCastTo(node.nodeType(), NodeType::IDEN)) {
      writeIden(+i, false);
      return;
   } else {
      TODO;
   }
}

void clef::CodeGenerator::writeProc(index<Scope> i) {
   debug_assert(i);

   out.write('{');
   ++indents;
   writeStmtSeq(i);
   --indents;
   if (tree[i].size()) {
      nl();
   }
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