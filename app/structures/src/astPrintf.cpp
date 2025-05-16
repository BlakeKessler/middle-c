#ifndef CLEF_AST_PRINTF_CPP
#define CLEF_AST_PRINTF_CPP

#include "SyntaxTree.hpp"
#include "astNode.hpp"
#include "pretty-print.hpp"
#include "io.hpp"

uint mcsl::writef(File& file, const clef::SyntaxTree& tree, char mode, FmtArgs args) {
   assume((mode | CASE_BIT) == 's');
   return writef(file, clef::astTNB<clef::astNode>{tree, 1, 0}, mode, args) + (file.write('\n'), 1);
}



uint mcsl::writef(mcsl::File& file, const clef::indenter i, char mode, FmtArgs fmt) {
   assume((mode | CASE_BIT) == 's');
   file.write('\n');
   uint charCount = i.indents * i.indentSize;
   file.write(mcsl::PAD_CHAR, charCount);
   return charCount + 1;
}

#define TNB(expr) clef::astTNB{obj.tree, expr, obj.indents}
#define TNB_INDENT(expr) clef::astTNB{obj.tree, expr, obj.indents + 1}
#define TNB_AST(expr) TNB((clef::index<const clef::astNode>)expr)
#define TNB_CAST(T) clef::astTNB<clef::T>(obj.tree, +obj.i, obj.indents)
#define TNB_CAST2(T, expr) clef::astTNB<clef::T>(obj.tree, expr, obj.indents)
#define TNB_CAST_INDENT(T) TNB_INDENT(clef::index<const clef::T>(obj.i))

//!TODO: probably shouldn't just return 0 without doing anything when printing with `%b`

#define NODE_CAST_TNB_WRITEF(T) case T::nodeType(): return writef(file, TNB(clef::index<const clef::T>(obj.i)), mode, args);
#include "MAP_MACRO.h"
uint mcsl::writef(File& file, const clef::astTNB<clef::astNode> obj, char mode, FmtArgs args) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   switch (obj->nodeType()) {
      MCSL_MAP(NODE_CAST_TNB_WRITEF, CLEF_ALL_AST_NODE_T)
      
      case NodeType::NONE:
      case NodeType::ERROR:
         UNREACHABLE;
   }
}
#include "MAP_MACRO_UNDEF.h"
#undef NODE_CAST_TNB_WRITEF


uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Literal> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Literal& lit = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      switch (lit.type()) {
         case LitType::NONE:
            mcsl::err_printf(FMT("ERROR: NULL CLEF LITERAL"));
            UNREACHABLE;

         case LitType::POINTER: return file.printf(FMT("%r"), (const void*)lit);

         case LitType::UINT: return file.printf(FMT("%u"), (ulong)lit);
         case LitType::SINT: return file.printf(FMT("%i"), (slong)lit);
         case LitType::FLOAT: return file.printf(FMT("%f"), (flong)lit);

         case LitType::CHAR: return file.printf(FMT("\'%c\'"), (char)lit);
         case LitType::STRING: [[fallthrough]];
         case LitType::INTERP_STR: [[fallthrough]];
         case LitType::FORMAT: [[fallthrough]];
         case LitType::REGEX: return file.printf(FMT("\"%s\""), (const str_slice)lit);

         case LitType::TYPEID: return writef(file, astTNB<Type>{obj.tree, (clef::index<const Type>)lit, obj.indents}, mode, fmt);
      }
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      uint charsPrinted = writef(file, +lit.type(), 'b', fmt); //write literal type
      switch (lit.type()) { //write literal
         case LitType::NONE: break;

         case LitType::POINTER: charsPrinted += writef(file, (const void*)lit, 'b', fmt); break;

         case LitType::UINT: charsPrinted += writef(file, (ulong)lit, 'b', fmt); break;
         case LitType::SINT: charsPrinted += writef(file, (slong)lit, 'b', fmt); break;
         case LitType::FLOAT: charsPrinted += writef(file, (flong)lit, 'b', fmt); break;

         case LitType::CHAR: charsPrinted += writef(file, (char)lit, 'b', fmt); break;
         case LitType::STRING: [[fallthrough]];
         case LitType::INTERP_STR: [[fallthrough]];
         case LitType::FORMAT: [[fallthrough]];
         case LitType::REGEX: charsPrinted += writef(file, (const str_slice)lit, 'b', fmt); break;

         case LitType::TYPEID: charsPrinted += writef(file, TNB((clef::index<const Type>)lit), 'b', fmt);
      }
      return charsPrinted;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Literal>"), mode);
   }
   UNREACHABLE;
}

#pragma region exprs

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::If> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const If& ifstmt = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      if (!ifstmt.condition()) { //else
         debug_assert(!ifstmt.elseStmt());
         return file.printf(FMT("%s;"), TNB(ifstmt.procedure()));
      }
      if (ifstmt.elseStmt()) { //if with else
         return file.printf(FMT("if (%s) %s else %s"), TNB(ifstmt.condition()), TNB(ifstmt.procedure()), TNB(ifstmt.elseStmt()));
      } else { //if without else
         return file.printf(FMT("if (%s) %s;"), TNB(ifstmt.condition()), TNB(ifstmt.procedure()));
      }
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), TNB(ifstmt.condition()), TNB(ifstmt.procedure()), TNB(ifstmt.elseStmt()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<If>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ForLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ForLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("for (%s) %s;"), TNB(loop.params()), TNB(loop.procedure()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(loop.params()), TNB(loop.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ForLoop>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ForLoopParams> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ForLoopParams& params = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("%s; %s; %s"), TNB(params.decl()), TNB(params.condition()), TNB(params.increment()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), TNB(params.decl()), TNB(params.condition()), TNB(params.increment()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ForLoopParams>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ForeachLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ForeachLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("foreach (%s) %s;"), TNB(loop.params()), TNB(loop.procedure()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(loop.params()), TNB(loop.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ForeachLoop>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ForeachLoopParams> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ForeachLoopParams& params = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("%s : %s"), TNB(params.iterator()), TNB(params.target()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(params.iterator()), TNB(params.target()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ForeachLoopParams>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::WhileLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const WhileLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("while (%s) %s;"), TNB(loop.condition()), TNB(loop.procedure()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(loop.condition()), TNB(loop.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<WhileLoop>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::DoWhileLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const DoWhileLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("do %s while (%s);"), TNB(loop.procedure()), TNB(loop.condition()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(loop.procedure()), TNB(loop.condition()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<DoWhileLoop>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Switch> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Switch& switchstmt = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("switch (%s) %s;"), TNB(switchstmt.condition()), TNB(switchstmt.cases()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(switchstmt.condition()), TNB(switchstmt.cases()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Switch>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Match> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Match& matchstmt = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("switch (%s) %s;"), TNB(matchstmt.condition()), TNB(matchstmt.cases()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(matchstmt.condition()), TNB(matchstmt.cases()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Match>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::TryCatch> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const TryCatch& trycatch = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("try %s catch (%s) %s;"), TNB(trycatch.procedure()), TNB(trycatch.err()), TNB(trycatch.errHandler()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), TNB(trycatch.procedure()), TNB(trycatch.err()), TNB(trycatch.errHandler()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<TryCatch>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Decl> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Decl& decl = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("let %s %s"), TNB(decl.type()), TNB(decl.name()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(decl.type()), TNB(decl.name()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Decl>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::TypeDecl> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const TypeDecl& decl = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      uint charCount = file.printf(FMT("%s %s"), TNB(decl.objType()), TNB(decl.name()));
      if (decl.spec()) {
         switch (decl.specType()) {
            case NodeType::OBJ_TYPE_SPEC:
               charCount += file.printf(FMT(" {%s};"), TNB_INDENT(decl.objSpec()));
               break;
            case NodeType::INTERFACE_SPEC:
               charCount += file.printf(FMT(" {%s};"), TNB_INDENT(decl.ifaceSpec()));
               break;
            case NodeType::NAMESPACE_SPEC:
               charCount += file.printf(FMT(" {%s};"), TNB_INDENT(decl.nsSpec()));
               break;
            default:
               UNREACHABLE;
         }
      } else {
         charCount += file.printf(FMT(";"));
      }
      return charCount;
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      uint charCount = file.printf(FMT("%b%b%b"), TNB(decl.objType()), TNB(decl.name()), +decl.specType());
      switch (decl.specType()) {
         case NodeType::OBJ_TYPE_SPEC:
            charCount += file.printf(FMT("%b"), TNB_INDENT(decl.objSpec()));
            break;
         case NodeType::INTERFACE_SPEC:
            charCount += file.printf(FMT("%b"), TNB_INDENT(decl.ifaceSpec()));
            break;
         case NodeType::NAMESPACE_SPEC:
            charCount += file.printf(FMT("%b"), TNB_INDENT(decl.nsSpec()));
            break;
         default:
            UNREACHABLE;
      }
      return charCount;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<TypeDecl>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Asm> obj, char mode, FmtArgs fmt) {
   clef::throwError(clef::ErrCode::PARSER_NOT_IMPLEMENTED, FMT("inline ASM is not implemented yet"));
}

#pragma endregion exprs

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Stmt> obj, char mode, FmtArgs fmt) {
   if ((mode | CASE_BIT) == 's') {
      return file.printf(FMT("%s;"), TNB_CAST(Expr));
   } else if ((mode | CASE_BIT) == 'b') {
      return writef(file, TNB_CAST(Expr), mode, fmt);
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Stmt>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Expr> obj, char mode, FmtArgs fmt) { //!TODO: add parens if relative precedence indicates that it would be required
   using namespace clef;
   using enum OpID;
   if (!obj) {
      return 0;
   }
   
   #define BIN(op) file.printf(FMT("%s" op "%s"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()))
   const clef::Expr& expr = *obj;
   if ((mode | CASE_BIT) == 's') {
      switch (expr.opID()) {
         case NULL:
            debug_assert(!expr.rhs() && !expr.extra());
            return file.printf(FMT("%s"), TNB_AST(expr.lhs()));

         case ESCAPE: return file.printf(FMT("\\"));
         case EOS   : return file.printf(FMT(";"));

         case STRING       : UNREACHABLE;
         case CHAR         : UNREACHABLE;
         case INTERP_STRING: UNREACHABLE;

         case LINE_CMNT       : UNREACHABLE;
         case BLOCK_CMNT      : UNREACHABLE;
         case BLOCK_CMNT_OPEN : UNREACHABLE;
         case BLOCK_CMNT_CLOSE: UNREACHABLE;

         case CALL_INVOKE: //parens
            return file.printf(FMT("%s(%s)"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));
         case SUBSCRIPT_INVOKE: //square brackets
            return file.printf(FMT("%s[%s]"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));
         case LIST_INVOKE: //curly brackets
            return file.printf(FMT("%s{%s}"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));
         case SPECIALIZER_INVOKE: //triangle brackets
            return file.printf(FMT("%s<%s>"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));

         case CALL_OPEN        : UNREACHABLE;
         case CALL_CLOSE       : UNREACHABLE;
         case SUBSCRIPT_OPEN   : UNREACHABLE;
         case SUBSCRIPT_CLOSE  : UNREACHABLE;
         case LIST_OPEN        : UNREACHABLE;
         case LIST_CLOSE       : UNREACHABLE;
         case SPECIALIZER_OPEN : UNREACHABLE;
         case SPECIALIZER_CLOSE: UNREACHABLE;

         case CHAR_INVOKE      : UNREACHABLE;
         case STR_INVOKE       : UNREACHABLE;

         case INTERP_STR_INVOKE: TODO;
         case TERNARY_INVOKE   :
            return file.printf(FMT("%s ? %s : %s"), TNB_CAST2(Expr, expr.lhs()), TNB_CAST2(Expr, expr.rhs()), TNB_CAST2(Expr, expr.extra()));

         case PREPROCESSOR: TODO;

         case SCOPE_RESOLUTION: UNREACHABLE;

         case INC: //increment
            if (expr.lhs()) { //!NOTE: might have these two backwards
               return file.printf(FMT("%s++"), TNB_AST(expr.lhs()));
            } else {
               debug_assert(expr.rhs());
               return file.printf(FMT("++%s"), TNB_AST(expr.rhs()));
            }
            UNREACHABLE;
         case DEC: //decrement
         if (expr.lhs()) { //!NOTE: might have these two backwards
            return file.printf(FMT("%s--"), TNB_AST(expr.lhs()));
         } else {
            debug_assert(expr.rhs());
            return file.printf(FMT("--%s"), TNB_AST(expr.rhs()));
         }
         UNREACHABLE;

         case MEMBER_ACCESS    : //.
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            return BIN(".");
         case PTR_MEMBER_ACCESS: // ->
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            return BIN("->");
         case METHOD_PTR       : // .*
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            return BIN(".*");
         case ARROW_METHOD_PTR : // ->*
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            return BIN("->*");

         case RANGE : return BIN(".."); //relies on printing null nodes being a no-op (not an error)
         case SPREAD: return BIN("..."); //relies on printing null nodes being a no-op (not an error)
         
         case ADD: return BIN(" + ");
         case SUB: return BIN(" - ");
         case MUL: return BIN(" * ");
         case DIV: return BIN(" / ");
         case MOD: return BIN(" %% ");
         case EXP: return BIN(" ^^ ");
         
         case LOGICAL_NOT: return file.printf(FMT("!%s"), TNB_AST(expr.lhs()));
         case LOGICAL_AND: return BIN(" && ");
         case LOGICAL_OR : return BIN(" || ");

         case BIT_NOT    : return file.printf(FMT("~%s"), TNB_AST(expr.lhs()));
         case BIT_AND    : return BIN(" & ");
         case BIT_OR     : return BIN(" | ");
         case BIT_XOR    : return BIN(" ^ ");
         case SHIFT_LEFT : return BIN(" << ");
         case SHIFT_RIGHT: return BIN(" >> ");
         
         
         case UNIQUE_PTR: TODO;
         case SHARED_PTR: TODO;
         case WEAK_PTR  : TODO;
         case ITERATOR  : TODO;
         

         case THREE_WAY_COMP: return BIN(" <=> ");
         case LESSER        : return BIN(" < ");
         case GREATER       : return BIN(" > ");
         case LESSER_OR_EQ  : return BIN(" <= ");
         case GREATER_OR_EQ : return BIN(" >= ");

         case IS_EQUAL  : return BIN(" == ");
         case IS_UNEQUAL: return BIN(" != ");
         // case IS_EQUAL_STRICT  : return BIN(" === ");
         // case IS_UNEQUAL_STRICT: return BIN(" !== ");

         case COALESCE: return BIN(" ?? "); //null coalescing

         case INLINE_IF  : UNREACHABLE; //ternary operator opener
         case INLINE_ELSE: UNREACHABLE; //ternary operator closer

         case ASSIGN: return BIN(" = ");
         // case CONST_ASSIGN: return BIN(" := ");
         case ADD_ASSIGN: return BIN(" += ");
         case SUB_ASSIGN: return BIN(" -= ");
         case MUL_ASSIGN: return BIN(" *= ");
         case DIV_ASSIGN: return BIN(" /= ");
         case MOD_ASSIGN: return BIN(" %%= ");
         case EXP_ASSIGN: return BIN(" ^^= ");
         case SHL_ASSIGN: return BIN(" <<= ");
         case SHR_ASSIGN: return BIN(" >>= ");
         case AND_ASSIGN: return BIN(" &= ");
         case XOR_ASSIGN: return BIN(" ^= ");
         case OR_ASSIGN : return BIN(" |= ");
         case COALESCE_ASSIGN: return BIN(" ??= ");

         case COMMA: return BIN(", ");



         //keyword pseudo-operators
         case FOR      : return writef(file, TNB_CAST(ForLoop),     mode, fmt);
         case FOREACH  : return writef(file, TNB_CAST(ForeachLoop), mode, fmt);
         case WHILE    : return writef(file, TNB_CAST(WhileLoop),   mode, fmt);
         case DO_WHILE : return writef(file, TNB_CAST(DoWhileLoop), mode, fmt);
         case IF       : return writef(file, TNB_CAST(If),          mode, fmt);
         case SWITCH   : return writef(file, TNB_CAST(Switch),      mode, fmt);
         case MATCH    : return writef(file, TNB_CAST(Match),       mode, fmt);
         case TRY_CATCH: return writef(file, TNB_CAST(TryCatch),    mode, fmt);
         case ASM      : return writef(file, TNB_CAST(Asm),         mode, fmt);
         case LET      : return writef(file, TNB_CAST(Decl),        mode, fmt);

         case GOTO: return file.printf(FMT("goto %s"), TNB_AST(expr.lhs()));
         case GOTO_CASE: return file.printf(FMT("goto case %s"), TNB_AST(expr.lhs()));

         case BREAK   : return file.printf(FMT("break"));
         case CONTINUE: return file.printf(FMT("continue"));
         
         case THROW        : return file.printf(FMT("throw %s"), TNB_AST(expr.lhs()));
         case ASSERT       : return file.printf(FMT("assert %s"), TNB_AST(expr.lhs()));
         case DEBUG_ASSERT : return file.printf(FMT("debug_assert %s"), TNB_AST(expr.lhs()));
         case STATIC_ASSERT: return file.printf(FMT("static_assert %s"), TNB_AST(expr.lhs()));
         case ASSUME       : return file.printf(FMT("assume %s"), TNB_AST(expr.lhs()));
         case RETURN       : return file.printf(FMT("return %s"), TNB_AST(expr.lhs()));

         case ALIAS:
            if (expr.rhs()) {
               return file.printf(FMT("using %s = %s"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));
            } else {
               return file.printf(FMT("using %s"), TNB_AST(expr.lhs()));
            }
      }
   } else if ((mode | CASE_BIT) == 'b') {
      switch (expr.opID()) {
         case FOR      : return writef(file, TNB_CAST(ForLoop),     mode, fmt);
         case FOREACH  : return writef(file, TNB_CAST(ForeachLoop), mode, fmt);
         case WHILE    : return writef(file, TNB_CAST(WhileLoop),   mode, fmt);
         case DO_WHILE : return writef(file, TNB_CAST(DoWhileLoop), mode, fmt);
         case IF       : return writef(file, TNB_CAST(If),          mode, fmt);
         case SWITCH   : return writef(file, TNB_CAST(Switch),      mode, fmt);
         case MATCH    : return writef(file, TNB_CAST(Match),       mode, fmt);
         case TRY_CATCH: return writef(file, TNB_CAST(TryCatch),    mode, fmt);
         case ASM      : return writef(file, TNB_CAST(Asm),         mode, fmt);
         case LET      : return writef(file, TNB_CAST(Decl),        mode, fmt);

         default:
            return file.printf(FMT("%b%b%b%b%b%b"),
               +expr.lhsType(), TNB_AST(expr.lhs()),
               +expr.rhsType(), TNB_AST(expr.rhs()),
               +expr.extraType(), TNB_AST(expr.extra())
            );
      }
   }
   UNREACHABLE;
   #undef BIN
}


uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Scope> obj, char mode, FmtArgs fmt) {
   if ((mode | CASE_BIT) == 's') {
      return file.printf(FMT("{%s%S}"), TNB_CAST_INDENT(StmtSeq), obj.indents);
   } else if ((mode | CASE_BIT) == 'b') {
      return writef(file, TNB_CAST(StmtSeq), mode, fmt);
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Scope>"), mode);
   }
   UNREACHABLE;
}


uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Identifier> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Identifier& iden = *obj;
   if ((mode | CASE_BIT) == 's') {
      if (+iden.keywordID()) { //keyword
         return file.printf(FMT("%s"), toString(iden.keywordID()));
      }

      uint charsPrinted = 0;
      if (iden.scopeName()) {
         charsPrinted += file.printf(FMT("%s::"), TNB(iden.scopeName()));
      }
      debug_assert(iden.name().size() || !iden.name().data());
      charsPrinted += file.printf(FMT("%s"), iden.name());
      if (iden.specializer()) {
         charsPrinted += file.printf(FMT("<%s>"), TNB(iden.specializer()));
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      return writef(file, TNB_CAST(StmtSeq), mode, fmt);
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Scope>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Type> obj, char mode, FmtArgs fmt) {
   return writef(file, TNB_CAST(Identifier), mode, fmt);
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Variable> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Variable& var = *obj;
   if ((mode | CASE_BIT) == 's') {
      return file.printf(FMT("%s %s"), TNB(var.type()), TNB_CAST(Identifier));
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b"), TNB(var.type()), TNB_CAST(Identifier));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Variable>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Function> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Function& func = *obj;
   if ((mode | CASE_BIT) == 's') {
      return file.printf(FMT("func %s%s %s;"), TNB_CAST(Identifier), TNB(func.signature()), TNB(func.procedure()));
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b%b"), TNB_CAST(Identifier), TNB(func.signature()), TNB(func.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Function>"), mode);
   }
   UNREACHABLE;
}

#pragma region lists

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ArgList> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   if ((mode | CASE_BIT) == 's' || (mode | CASE_BIT) == 'b') {
      auto span = obj->span();
      uint charsPrinted = 0;
      for (uint i = 0; i < span.size();) {
         charsPrinted += writef(file, TNB(span[i]), mode, fmt);
         if (++i < span.size() && (mode | CASE_BIT) == 's' && !fmt.altMode) {
            charsPrinted += writef(file, FMT(", "), mode, fmt);
         }
      }
      return charsPrinted;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ArgList>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ParamList> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   if ((mode | CASE_BIT) == 's' || (mode | CASE_BIT) == 'b') {
      auto span = obj->span();
      uint charsPrinted = 0;
      for (uint i = 0; i < span.size();) {
         charsPrinted += writef(file, TNB(span[i]), mode, fmt);
         if (++i < span.size() && (mode | CASE_BIT) == 's' && !fmt.altMode) {
            charsPrinted += writef(file, FMT(", "), mode, fmt);
         }
      }
      return charsPrinted;
   }
   else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ParamList>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::SpecList> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const SpecList& list = *obj;
   if ((mode | CASE_BIT) == 's' || (mode | CASE_BIT) == 'b') {
      if (list.isDecl()) {
         return writef(file, TNB_CAST(ParamList), mode, fmt);
      } else {
         return writef(file, TNB_CAST(ArgList), mode, fmt);
      }
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<SpecList>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::StmtSeq> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const arr_span<clef::index<Stmt>> span = obj->span();
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      for (uint i = 0; i < span.size(); ++i) {
         charsPrinted += file.printf(FMT("%S%s"), obj.indents, TNB(span[i]));
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      auto span = obj->span();
      uint charsPrinted = 0;
      for (uint i = 0; i < span.size(); ++i) {
         charsPrinted += writef(file, TNB(span[i]), mode, fmt);
      }
      return charsPrinted;
   }
   else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<StmtSeq>"), mode);
   }
   UNREACHABLE;
}

#pragma endregion lists

#pragma region types

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::VariadicParam> obj, char mode, FmtArgs fmt) {
   if ((mode | CASE_BIT) == 's') {
      return file.printf(FMT("%s..."), TNB_CAST(Type));
   } else if ((mode | CASE_BIT) == 'b') {
      return writef(file, TNB_CAST(Type), mode, fmt);
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<VariadicParam>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Union> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Union& spec = *obj;

   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = file.printf(FMT("union %s"), TNB_CAST(Identifier));
      if (!spec.members()) { //forward declaration
         charsPrinted += file.printf(FMT(";"));
         return charsPrinted;
      }

      auto span = obj.tree[spec.members()].span();
      indenter membIndents{obj.indents + 1};

      charsPrinted += file.printf(FMT(" {"));
      for (uint i = 0; i < span.size(); ++i) { //print members
         charsPrinted += file.printf(FMT("%S%s;"), membIndents, TNB(span[i]));
      }
      charsPrinted += file.printf(FMT("%S};"), obj.indents);
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b"), TNB_CAST(Identifier), TNB(spec.members()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Union>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Enum> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Enum& spec = *obj;

   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = file.printf(FMT("enum %s"), TNB_CAST(Identifier));
      if (spec.baseType()) {
         charsPrinted += file.printf(FMT(" : %s"), TNB(spec.baseType()));
      }
      if (!spec.enumerators()) { //forward declaration
         charsPrinted += file.printf(FMT(";"));
         return charsPrinted;
      }

      auto span = obj.tree[spec.enumerators()].span();
      indenter membIndents{obj.indents + 1};

      charsPrinted += file.printf(FMT(" {"));
      for (uint i = 0; i < span.size(); ++i) { //print enumerators
         charsPrinted += file.printf(FMT("%S%s"), membIndents, TNB_CAST2(Identifier, +span[i]));
         if (obj.tree[span[i]].val()) {
            charsPrinted += file.printf(FMT(" = %s,"), TNB(obj.tree[span[i]].val()));
         } else {
            charsPrinted += file.printf(FMT(","));
         }
      }
      charsPrinted += file.printf(FMT("%S};"), obj.indents);
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b%b"), TNB_CAST(Identifier), TNB(spec.baseType()), TNB(spec.enumerators()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Enum>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Mask> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Mask& spec = *obj;

   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = file.printf(FMT("mask %s"), TNB_CAST(Identifier));
      if (spec.baseType()) {
         charsPrinted += file.printf(FMT(" : %s"), TNB(spec.baseType()));
      }
      if (!spec.enumerators()) { //forward declaration
         charsPrinted += file.printf(FMT(";"));
         return charsPrinted;
      }

      auto span = obj.tree[spec.enumerators()].span();
      indenter membIndents{obj.indents + 1};
      
      charsPrinted += file.printf(FMT(" {"));
      for (uint i = 0; i < span.size(); ++i) { //print enumerators
         charsPrinted += file.printf(FMT("%S%s"), membIndents, TNB_CAST2(Identifier, +span[i]));
         if (obj.tree[span[i]].val()) {
            charsPrinted += file.printf(FMT(" = %s,"), TNB(obj.tree[span[i]].val()));
         } else {
            charsPrinted += file.printf(FMT(","));
         }
      }
      charsPrinted += file.printf(FMT("%S};"), obj.indents);
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b%b"), TNB_CAST(Identifier), TNB(spec.baseType()), TNB(spec.enumerators()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Mask>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::FuncSig> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const FuncSig& spec = *obj;

   if ((mode | CASE_BIT) == 's') {
      return file.printf(FMT("(%s) -> %s"), TNB(spec.params()), TNB(spec.returnType()));
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b"), TNB(spec.params()), TNB(spec.returnType()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<FuncSig>"), mode);
   }
   UNREACHABLE;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::FundType> obj, char mode, FmtArgs fmt) {
   TODO;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::GenericType> obj, char mode, FmtArgs fmt) {
   TODO;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Class> obj, char mode, FmtArgs fmt) {
   TODO;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Struct> obj, char mode, FmtArgs fmt) {
   TODO;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Interface> obj, char mode, FmtArgs fmt) {
   TODO;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Namespace> obj, char mode, FmtArgs fmt) {
   TODO;
}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::SwitchCases> obj, char mode, FmtArgs fmt) {
   TODO;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::MatchCases> obj, char mode, FmtArgs fmt) {
   TODO;
}

#pragma endregion types

#undef TNB_CAST
#undef TNB_AST
#undef TNB_INDENT
#undef TNB

#endif //CLEF_AST_PRINTF_CPP