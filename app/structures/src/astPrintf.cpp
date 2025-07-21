#ifndef CLEF_AST_PRINTF_CPP
#define CLEF_AST_PRINTF_CPP

#include "SyntaxTree.hpp"
#include "astNode.hpp"
#include "pretty-print.hpp"
#include "OperatorData.hpp"
#include "io.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

//print an AST by printing its root node
uint mcsl::writef(File& file, const clef::SyntaxTree& tree, char mode, FmtArgs args) {
   assume((mode | CASE_BIT) == 's');
   return writef(file, clef::astTNB<clef::astNode>{tree, 1, 0}, mode, args) + (file.write('\n'), 1);
}


//indent
uint mcsl::writef(mcsl::File& file, const clef::indenter i, char mode, FmtArgs fmt) {
   assume((mode | CASE_BIT) == 's');
   file.write('\n');
   uint charCount = i.indents * i.indentSize;
   file.write(mcsl::PAD_CHAR, charCount);
   return charCount + 1;
}

//helper macros for printing AST nodes
#define TNB(expr) clef::astTNB{obj.tree, expr, obj.indents}
#define TNB_INDENT(expr) clef::astTNB{obj.tree, expr, obj.indents + 1}
#define TNB_AST(expr) TNB((clef::index<const clef::astNode>)expr)
#define TNB_CAST(T) clef::astTNB<clef::T>(obj.tree, +obj.i, obj.indents)
#define TNB_CAST2(T, expr) clef::astTNB<clef::T>(obj.tree, expr, obj.indents)
#define TNB_CAST_INDENT(T) TNB_INDENT(clef::index<const clef::T>(obj.i))
#define TNB_CAST_INDENT2(T, expr) TNB_INDENT(clef::index<const clef::T>(expr))

//helper macros for printing type specifications
#define TTsB(expr) clef::astTTsB{obj.tree, expr, obj.indents}
#define TTsB_INDENT(expr) clef::astTTsB{obj.tree, expr, obj.indents + 1}

//helper macros for printing symbols
#define TSB(expr) clef::astTSB{obj.tree, expr, obj.indents}
#define TSB_INDENT(expr) clef::astTSB{obj.tree, expr, obj.indents + 1}

//helper macros for printing indirection tables
#define TItB(expr) clef::astTItB{obj.tree, expr, obj.indents}
#define TItB_INDENT(expr) clef::astTItB{obj.tree, expr, obj.indents + 1}

//!TODO: probably shouldn't just return 0 without doing anything when printing with `%b`

//print a generic AST node
//delegates to the type indicated by the node's nodeType
//passes any format codes on
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

//print a literal
//ignores all format codes
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

         //numeric
         case LitType::UINT: return file.printf(FMT("%u%s"), (ulong)lit, getSuffix(lit.typeName()));
         case LitType::SINT: return file.printf(FMT("%i%s"), (slong)lit, getSuffix(lit.typeName()));
         case LitType::FLOAT: return file.printf(FMT("%f%s"), (flong)lit, getSuffix(lit.typeName()));

         case LitType::BOOL: return file.printf(FMT("%s"), (bool)lit);

         //text
         case LitType::CHAR: return file.printf(FMT("\'%c\'"), (char)lit);
         case LitType::STRING: [[fallthrough]];
         case LitType::INTERP_STR: [[fallthrough]];
         case LitType::FORMAT: [[fallthrough]];
         case LitType::REGEX: return file.printf(FMT("\"%#s\""), (const str_slice)lit);

         //type
         case LitType::TYPEID: return file.printf(FMT("%s"), TTsB((const TypeSpec*)lit));
      }
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      uint charsPrinted = writef(file, +lit.type(), 'b', fmt); //write literal type
      switch (lit.type()) { //write literal
         case LitType::NONE: break;

         case LitType::POINTER: charsPrinted += writef(file, (const void*)lit, 'b', fmt); break;

         case LitType::UINT: charsPrinted += writef(file, (ulong)lit, 'b', fmt); break;
         case LitType::SINT: charsPrinted += writef(file, (slong)lit, 'b', fmt); break;
         case LitType::FLOAT: charsPrinted += writef(file, (flong)lit, 'b', fmt); break;

         case LitType::BOOL: charsPrinted += writef(file, (bool)lit, 'b', fmt); break;
         case LitType::CHAR: charsPrinted += writef(file, (char)lit, 'b', fmt); break;
         case LitType::STRING: [[fallthrough]];
         case LitType::INTERP_STR: [[fallthrough]];
         case LitType::FORMAT: [[fallthrough]];
         case LitType::REGEX: charsPrinted += writef(file, (const str_slice)lit, 'b', fmt); break;

         case LitType::TYPEID: TODO; //charsPrinted += writef(file, TNB((clef::index<const Type>)lit), 'b', fmt);
      }
      return charsPrinted;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Literal>"), mode);
   }
   UNREACHABLE;
}

#pragma region exprs

//print an if statement
//altMode: prints as an else-if
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::If> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const If& ifstmt = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      if (!ifstmt.condition()) { //else
         debug_assert(!ifstmt.elseStmt());
         return file.printf(FMT(" else %s"), TNB(ifstmt.procedure()));
      }
      return file.printf(FMT(fmt.altMode ? " else if (%s) %s%#s" : "if (%s) %s%#s"), TNB(ifstmt.condition()), TNB(ifstmt.procedure()), TNB(ifstmt.elseStmt()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), TNB(ifstmt.condition()), TNB(ifstmt.procedure()), TNB(ifstmt.elseStmt()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<If>"), mode);
   }
   UNREACHABLE;
}

//print a for loop
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ForLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ForLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("for (%s;% s;% s) %s"), TNB(loop.decl()), TNB(loop.condition()), TNB(loop.increment()), TNB(loop.procedure()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b%b"), TNB(loop.decl()), TNB(loop.condition()), TNB(loop.increment()), TNB(loop.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ForLoop>"), mode);
   }
   UNREACHABLE;
}
//print a foreach loop
//ignores all foramt codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::ForeachLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const ForeachLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("foreach (%#s : %s) %s"), TNB(loop.iterator()), TNB(loop.target()), TNB(loop.procedure()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), TNB(loop.iterator()), TNB(loop.target()), TNB(loop.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ForeachLoop>"), mode);
   }
   UNREACHABLE;
}
//print a while loop
//ignores all foramt codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::WhileLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const WhileLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("while (%s) %s"), TNB(loop.condition()), TNB(loop.procedure()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(loop.condition()), TNB(loop.procedure()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<WhileLoop>"), mode);
   }
   UNREACHABLE;
}
//print a do-while loop
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::DoWhileLoop> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const DoWhileLoop& loop = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("do %s while (%s)"), TNB(loop.procedure()), TNB(loop.condition()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(loop.procedure()), TNB(loop.condition()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<DoWhileLoop>"), mode);
   }
   UNREACHABLE;
}
//print a switch statement
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Switch> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Switch& switchstmt = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("switch (%s) {%s%S}"), TNB(switchstmt.condition()), TNB_INDENT(switchstmt.cases()), obj.indents);
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(switchstmt.condition()), TNB(switchstmt.cases()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Switch>"), mode);
   }
   UNREACHABLE;
}
//print a match statement
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Match> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Match& matchstmt = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("match (%s) %s"), TNB(matchstmt.condition()), TNB(matchstmt.cases()));
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(matchstmt.condition()), TNB(matchstmt.cases()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Match>"), mode);
   }
   UNREACHABLE;
}

//print a declaration
//altMode: suppress printing of `let` prefix
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Decl> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Decl& decl = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      uint charsPrinted = 0;
      if (!fmt.altMode) {
         charsPrinted += file.printf(FMT("let "));
      }
      if (decl.value()) {
         charsPrinted += file.printf(FMT("%s% s = %s"), TNB(decl.type()), TNB(decl.name()), TNB(decl.value()));
      } else {
         charsPrinted += file.printf(FMT("%s% s"), TNB(decl.type()), TNB(decl.name()));
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), TNB(decl.type()), TNB(decl.name()), TNB_AST(decl.value()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Decl>"), mode);
   }
   UNREACHABLE;
}
//print a type declaration
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::TypeDecl> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const TypeDecl& decl = *obj;
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      uint charCount = file.printf(FMT("%s %s"), toString(obj.tree[decl.name()].symbol()->symbolType()), TNB(decl.name()));
      if (decl.decl()) {
         if (decl.decl() == decl.name()) {
            charCount += file.printf(FMT(" {%s}"), TTsB(obj.tree[decl.decl()].symbol()->type()));
         } else {
            charCount += file.printf(FMT(" = %s"), TNB(decl.decl()));
         }
      }
      return charCount;
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b"), TNB(decl.name()), TNB(decl.decl()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<TypeDecl>"), mode);
   }
   UNREACHABLE;
}

//print an inline assembly block
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Asm> obj, char mode, FmtArgs fmt) {
   TODO;
}

#pragma endregion exprs

//print a statement
//delegates to Expr
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Stmt> obj, char mode, FmtArgs fmt) {
   if (!obj) {
      return 0;
   }
   if ((mode | CASE_BIT) == 's') {
      if (obj->opID() == clef::OpID::LABEL_DELIM && !obj->rhs() && !obj->extra()) { //special case for labels
         return file.printf(FMT("%s"), TNB_CAST(Expr));
      }
      return file.printf(FMT("%s;"), TNB_CAST(Expr));
   } else if ((mode | CASE_BIT) == 'b') {
      return writef(file, TNB_CAST(Expr), mode, fmt);
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Stmt>"), mode);
   }
   UNREACHABLE;
}

//print expressions
//may delegate to subtypes based on the operator 
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Expr> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   using enum OpID;
   if (!obj) {
      return 0;
   }
   
   #define SUBEXPR(operand, expr, b, a) file.printf(operand##NeedsParens ? FMT(b "(%s)" a) : FMT(b "%s" a), expr)
   #define BIN(op) SUBEXPR(lhs, TNB_AST(expr.lhs()),,op) + SUBEXPR(rhs, TNB_AST(expr.rhs()),,)
   #define BIN_OR_UN(op) if (expr.lhs() && expr.rhs()) { charsPrinted += BIN(" " op " "); break; } else { charsPrinted += BIN(op); break; } UNREACHABLE
   const clef::Expr& expr = *obj;
   if ((mode | CASE_BIT) == 's') {
      bool lhsNeedsParens = false;
      bool rhsNeedsParens = false;
      bool extraNeedsParens = false;
      auto [selfPrec, selfIsLeftAssoc] = PRECS(expr.opID(), expr.lhs(), expr.rhs());
      if (canDownCastTo(NodeType::EXPR, expr.lhsType()) && expr.lhs()) {
         const clef::Expr& lhs = obj.tree[(clef::index<Expr>)expr.lhs()];
         auto [prec, isLeftAssoc] = PRECS(lhs.opID(), lhs.lhs(), lhs.rhs());
         lhsNeedsParens = prec && (selfPrec > prec || (!isLeftAssoc && selfPrec == prec));
      }
      if (canDownCastTo(NodeType::EXPR, expr.rhsType()) && expr.rhs()) {
         const clef::Expr& rhs = obj.tree[(clef::index<Expr>)expr.rhs()];
         auto [prec, isLeftAssoc] = PRECS(rhs.opID(), rhs.lhs(), rhs.rhs());
         rhsNeedsParens = prec && (selfPrec > prec || (isLeftAssoc && selfPrec == prec));
      }
      if (canDownCastTo(NodeType::EXPR, expr.extraType()) && expr.extra()) {
         const clef::Expr& extra = obj.tree[(clef::index<Expr>)expr.extra()];
         auto [prec, isLeftAssoc] = PRECS(extra.opID(), extra.lhs(), extra.rhs());
         extraNeedsParens = prec && selfPrec > prec;
      }

      uint charsPrinted = 0;
      if (fmt.padForPosSign) {
         charsPrinted += file.printf(FMT(" "));
      }

      switch (expr.opID()) {
         case NULL:
            debug_assert(!expr.rhs() && !expr.extra());
            charsPrinted += file.printf(FMT("%s"), TNB_AST(expr.lhs()));
            break;

         case ESCAPE: charsPrinted += file.printf(FMT("\\")); break;
         case EOS   : UNREACHABLE; //return file.printf(FMT(";"));

         case STRING       : UNREACHABLE;
         case CHAR         : UNREACHABLE;
         case INTERP_STRING: UNREACHABLE;

         case LINE_CMNT       : UNREACHABLE;
         case BLOCK_CMNT      : UNREACHABLE;
         case BLOCK_CMNT_OPEN : UNREACHABLE;
         case BLOCK_CMNT_CLOSE: UNREACHABLE;

         case ATTRIBUTE:
            if (expr.extra()) {
               charsPrinted += file.printf(FMT("%s "), TNB_AST(expr.extra()));
            }
            charsPrinted += file.printf(FMT("@%s"), TNB_AST(expr.lhs()));
            if (expr.rhs()) {
               charsPrinted += file.printf(FMT("(%s)"), TNB_AST(expr.rhs()));
            }
            charsPrinted += file.printf(FMT("% s"), TNB_AST(expr.extra2()));
            break;

         case CALL_INVOKE: //parens
            charsPrinted += file.printf(FMT("%s(%#s)"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs())); break;
         case SUBSCRIPT_INVOKE: //square brackets
            charsPrinted += file.printf(FMT("%s[%#s]"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs())); break;
         case LIST_INVOKE: //curly brackets
            charsPrinted += file.printf(FMT("%s{%#s}"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs())); break;
         case SPECIALIZER_INVOKE: //triangle brackets
            charsPrinted += file.printf(FMT("%s<:%#s:>"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs())); break;

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
            charsPrinted +=
               SUBEXPR(lhs, TNB_AST(expr.lhs()),, " ? ")
             + SUBEXPR(rhs, TNB_AST(expr.rhs()),, " : ")
             + SUBEXPR(extra, TNB_AST(expr.extra()),,);
            break;

         case PREPROCESSOR: TODO;

         case SCOPE_RESOLUTION: UNREACHABLE;

         case INC: return BIN("++"); //increment
         case DEC: return BIN("--"); //decrement

         case MEMBER_ACCESS    : //.
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            charsPrinted += BIN(".");
            break;
         case PTR_MEMBER_ACCESS: // ->
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            charsPrinted += BIN("->");
            break;
         case METHOD_PTR       : // .*
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            charsPrinted += BIN(".*");
            break;
         case ARROW_METHOD_PTR : // ->*
            debug_assert(expr.lhs());
            debug_assert(expr.rhs());
            charsPrinted += BIN("->*");
            break;

         case RANGE : charsPrinted += BIN(".."); break; //relies on printing null nodes being a no-op (not an error)
         case SPREAD: charsPrinted += BIN("..."); break; //relies on printing null nodes being a no-op (not an error)
         
         case ADD: BIN_OR_UN("+");
         case SUB: BIN_OR_UN("-");
         case MUL: BIN_OR_UN("*");
         case DIV: charsPrinted += BIN(" / "); break;
         case MOD: charsPrinted += BIN(" %% "); break;
         case EXP: charsPrinted += BIN(" ^^ "); break;
         
         case LOGICAL_NOT: charsPrinted += SUBEXPR(rhs, TNB_AST(expr.rhs()), "!",); break;
         case LOGICAL_AND: charsPrinted += BIN(" && "); break;
         case LOGICAL_OR : charsPrinted += BIN(" || "); break;

         case BIT_NOT    : charsPrinted += SUBEXPR(rhs, TNB_AST(expr.rhs()), "~",); break;
         case BIT_AND    : BIN_OR_UN("&");
         case BIT_OR     : charsPrinted += BIN(" | "); break;
         case BIT_XOR    : charsPrinted += BIN(" ^ "); break;
         case SHIFT_LEFT : charsPrinted += BIN(" << "); break;
         case SHIFT_RIGHT: charsPrinted += BIN(" >> "); break;
         

         case THREE_WAY_COMP: charsPrinted += BIN(" <=> "); break;
         case LESSER        : charsPrinted += BIN(" < "); break;
         case GREATER       : charsPrinted += BIN(" > "); break;
         case LESSER_OR_EQ  : charsPrinted += BIN(" <= "); break;
         case GREATER_OR_EQ : charsPrinted += BIN(" >= "); break;

         case IS_EQUAL  : charsPrinted += BIN(" == "); break;
         case IS_UNEQUAL: charsPrinted += BIN(" != "); break;
         // case IS_EQUAL_STRICT  : charsPrinted += BIN(" === "); break;
         // case IS_UNEQUAL_STRICT: charsPrinted += BIN(" !== "); break;

         case COALESCE: charsPrinted += BIN(" ?? "); break; //null coalescing

         case INLINE_IF  : UNREACHABLE; //ternary operator opener
         case LABEL_DELIM: charsPrinted += file.printf(FMT("%s:"), TNB_AST(expr.lhs())); break; //ternary operator closer

         case ASSIGN: charsPrinted += BIN(" = "); break;
         // case CONST_ASSIGN: charsPrinted += BIN(" := "); break;
         case ADD_ASSIGN: charsPrinted += BIN(" += "); break;
         case SUB_ASSIGN: charsPrinted += BIN(" -= "); break;
         case MUL_ASSIGN: charsPrinted += BIN(" *= "); break;
         case DIV_ASSIGN: charsPrinted += BIN(" /= "); break;
         case MOD_ASSIGN: charsPrinted += BIN(" %%= "); break;
         case EXP_ASSIGN: charsPrinted += BIN(" ^^= "); break;
         case SHL_ASSIGN: charsPrinted += BIN(" <<= "); break;
         case SHR_ASSIGN: charsPrinted += BIN(" >>= "); break;
         case AND_ASSIGN: charsPrinted += BIN(" &= "); break;
         case XOR_ASSIGN: charsPrinted += BIN(" ^= "); break;
         case OR_ASSIGN : charsPrinted += BIN(" |= "); break;
         case COALESCE_ASSIGN: charsPrinted += BIN(" ??= "); break;

         case COMMA: charsPrinted += BIN(", "); break;



         //keyword pseudo-operators
         case FOR      : charsPrinted += writef(file, TNB_CAST(ForLoop),     mode, fmt); break;
         case FOREACH  : charsPrinted += writef(file, TNB_CAST(ForeachLoop), mode, fmt); break;
         case WHILE    : charsPrinted += writef(file, TNB_CAST(WhileLoop),   mode, fmt); break;
         case DO_WHILE : charsPrinted += writef(file, TNB_CAST(DoWhileLoop), mode, fmt); break;
         case IF       : charsPrinted += writef(file, TNB_CAST(If),          mode, fmt); break;
         case SWITCH   : charsPrinted += writef(file, TNB_CAST(Switch),      mode, fmt); break;
         case MATCH    : charsPrinted += writef(file, TNB_CAST(Match),       mode, fmt); break;
         case ASM      : charsPrinted += writef(file, TNB_CAST(Asm),         mode, fmt); break;
         case LET      : charsPrinted += writef(file, TNB_CAST(Decl),        mode, fmt); break;
         case MAKE_TYPE: charsPrinted += writef(file, TNB_CAST(TypeDecl),    mode, fmt); break;

         case GOTO: charsPrinted += file.printf(FMT("goto %s"), TNB_AST(expr.lhs())); break;
         case GOTO_CASE:
            if (expr.lhs()) {
               charsPrinted += file.printf(FMT("goto case %s"), TNB_AST(expr.lhs()));
            } else {
               charsPrinted += file.printf(FMT("goto default"));
            }
            break;

         case BREAK   : charsPrinted += file.printf(FMT("break")); break;
         case CONTINUE: charsPrinted += file.printf(FMT("continue")); break;
         
         case THROW        : charsPrinted += file.printf(FMT("throw% s"), TNB_AST(expr.lhs())); break;
         case ASSERT       : charsPrinted += file.printf(FMT("assert %s"), TNB_AST(expr.lhs())); break;
         case DEBUG_ASSERT : charsPrinted += file.printf(FMT("debug_assert %s"), TNB_AST(expr.lhs())); break;
         case STATIC_ASSERT: charsPrinted += file.printf(FMT("static_assert %s"), TNB_AST(expr.lhs())); break;
         case ASSUME       : charsPrinted += file.printf(FMT("assume %s"), TNB_AST(expr.lhs())); break;
         case RETURN       : charsPrinted += file.printf(FMT("return% s"), TNB_AST(expr.lhs())); break;

         case ALIAS:
            if (expr.rhs()) {
               charsPrinted += file.printf(FMT("using %s = %s"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));
            } else {
               charsPrinted += file.printf(FMT("using %s"), TNB_AST(expr.lhs()));
            }
            break;
         
         case CAST      : [[fallthrough]];
         case UP_CAST   : [[fallthrough]];
         case DYN_CAST  : [[fallthrough]];
         case BIT_CAST  : [[fallthrough]];
         case CONST_CAST: 
            charsPrinted += file.printf(FMT("%s<%s>(%s)"), toString(expr.opID()), TNB_AST(expr.lhs()), TNB_AST(expr.rhs()));
            break;
         
         case DEF_FUNC_PARAMS   : charsPrinted += writef(file, TNB_CAST(FuncDef), mode, fmt); break;
         case DEF_MACRO_PARAMS  : charsPrinted += writef(file, TNB_CAST(MacroDef), mode, fmt); break;

         case PREPROC_IMPORT    : charsPrinted += file.printf(FMT("#import %s"), TNB_AST(expr.rhs())); break;
         case PREPROC_LINK      : charsPrinted += file.printf(FMT("#link %s"), TNB_AST(expr.rhs())); break;
         case PREPROC_EMBED     : charsPrinted += file.printf(FMT("#embed %s %s"), TNB_AST(expr.lhs()), TNB_AST(expr.rhs())); break;

         default: UNREACHABLE;
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      switch (expr.opID()) {
         #define CASE(T) case T::pseudoOpID(): return writef(file, +T::nodeType(), mode, fmt) + writef(file, TNB_CAST(T), mode, fmt)
         CASE(ForLoop);
         CASE(ForeachLoop);
         CASE(WhileLoop);
         CASE(DoWhileLoop);
         CASE(If);
         CASE(Switch);
         CASE(Match);
         CASE(Asm);
         CASE(Decl);
         CASE(FuncDef);
         CASE(MacroDef);
         CASE(TypeDecl);
         #undef CASE

         default:
            return file.printf(FMT("%b%b%b%b%b%b%b%b%b"),
               +expr.opID(),
               +expr.lhsType(), TNB_AST(expr.lhs()),
               +expr.rhsType(), TNB_AST(expr.rhs()),
               +expr.extraType(), TNB_AST(expr.extra()),
               +expr.extraType2(), TNB_AST(expr.extra2())
            );
      }
   }
   UNREACHABLE;
   #undef BIN_OR_UN
   #undef BIN
   #undef SUBEXPR
}

//print a scope
//padForPosSign: print a leading space for non-null nodes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Scope> obj, char mode, FmtArgs fmt) {
   if (!obj) { return 0; }
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      if (fmt.padForPosSign) {
         charsPrinted += file.printf(FMT(" "));
      }
      charsPrinted += file.printf(FMT("{%s%S}"), TNB_CAST_INDENT(StmtSeq), obj.indents);
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      return writef(file, TNB_CAST(StmtSeq), mode, fmt);
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Scope>"), mode);
   }
   UNREACHABLE;
}

//print a raw identifier
//padForPosSign: print a leading space for non-null nodes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::RawIdentifier> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const RawIdentifier iden = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      if (fmt.padForPosSign) {
         charsPrinted += file.printf(FMT(" "));
      }
      //name
      if (+iden.keywordID()) {
         charsPrinted += writef(file, toString(iden.keywordID()), mode, fmt);
      }
      else {
         charsPrinted += writef(file, iden.name(), mode, fmt);
      }
      //specializer
      if (iden.specializer()) {
         charsPrinted += file.printf(FMT("<:%s:>"), TNB_AST(iden.specializer()));
      }
      //return
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      TODO;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<RawIdentifier>"), mode);
   }
   UNREACHABLE;
}

//print an identifier
//padForPosSign: print a leading space for non-null nodes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Identifier> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const Identifier& iden = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      if (fmt.padForPosSign) {
         charsPrinted += file.printf(FMT(" "));
      }

      // if (iden.fundTypeID() != FundTypeID::null) { //FUNDAMENTAL TYPE
      //    charsPrinted += file.printf(FMT("%s%s"), iden.quals(), toString(iden.fundTypeID()));
      // }
      // else if (+iden.keywordID()) { //KEYWORD
      //    debug_assert(!iden.scopeName());
      //    if (isCast(iden.keywordID())) {
      //       debug_assert(iden.quals() == QualMask::_no_quals);
      //       charsPrinted += file.printf(FMT("%s<:%#s:>"), toString(iden.keywordID()), TNB(iden.specializer()));
      //    } else {
      //       debug_assert(!iden.specializer());
      //       charsPrinted += file.printf(FMT("%s%s"), iden.quals(), toString(iden.keywordID()));
      //    }
      // }
      // else { //NORMAL IDENTIFIER
         //qualifiers
         charsPrinted += file.printf(FMT("%s"), iden.quals());
         //parent scope
         if (iden.scopeName()) {
            charsPrinted += file.printf(FMT("%s::"), TNB(iden.scopeName()));
         }
         //name
         debug_assert(iden.name().begin() && iden.name().size());
         charsPrinted += file.printf(FMT("%s"), TSB(iden.symbol()));
         //specializer
         if (iden.specializer()) {
            charsPrinted += file.printf(FMT("<:%#s:>"), TNB(iden.specializer()));
         }
      // }

      if (+(iden.quals() & QualMask::VARIADIC)) {
         charsPrinted += file.printf(FMT("..."));
      }

      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      return file.printf(FMT("%b%b%b%b%b%b"), TSB(iden.symbol()), +iden.overloadIndex(), TNB(iden.specializer()), +iden.keywordID(), TNB(iden.scopeName()), +iden.quals());
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<Scope>"), mode);
   }
   UNREACHABLE;
}

//print a function definition
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::FuncDef> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }
   const FuncDef& funcDef = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      if (funcDef.attrs()) {
         charsPrinted += file.printf(FMT("%s "), TNB(funcDef.attrs()));
      }
      charsPrinted += file.printf(FMT("func% s(%#s) -> %#s% s"), TNB(funcDef.name()), TNB(funcDef.params()), TNB(obj.tree[funcDef.params()].extra()), TNB(funcDef.procedure()));
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      TODO;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<FuncDef>"), mode);
   }
   UNREACHABLE;
}
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::MacroDef> obj, char mode, FmtArgs fmt) {
   TODO;
}

#pragma region lists

//print an argument list
//forwards format codes when printing list elements
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
         if (++i < span.size() && (mode | CASE_BIT) == 's') {
            charsPrinted += writef(file, FMT(", "), mode, fmt);
         }
      }
      return charsPrinted;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<ArgList>"), mode);
   }
   UNREACHABLE;
}
//print a statement sequence
//ignores all format codes
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

//print the cases of a switch statement
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::SwitchCases> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }

   uint charsPrinted = 0;
   
   const SwitchCases& cases = *obj;
   const Scope& scope = obj.tree[cases.procedure()];
   auto caseBuf = cases.span();
   if ((mode | CASE_BIT) == 's') {
      auto scopeBuf = scope.span();
      indenter procIndents = obj.indents + 1;
      uint currCase = 0;
      for (uint i = 0; i < scopeBuf.size(); ++i) {
         if (caseBuf[currCase].second == scopeBuf[i]) {
            if (caseBuf[currCase].first) {
               charsPrinted += file.printf(FMT("%Scase %s:"), obj.indents, TNB(caseBuf[currCase].first));
            } else {
               charsPrinted += file.printf(FMT("%Sdefault:"), obj.indents, TNB(caseBuf[currCase].first));
            }
            ++currCase;
         }
         charsPrinted += file.printf(FMT("%S%s"), procIndents, TNB(scopeBuf[i]));
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      charsPrinted += file.printf(FMT("%b"), caseBuf.size());
      for (uint i = 0; i < caseBuf.size(); ++i) {
         charsPrinted += file.printf(FMT("%b%b"), TNB(caseBuf[i].first), TNB(caseBuf[i].second));
      }
      charsPrinted += file.printf(FMT("%b"), TNB(cases.procedure()));
      return charsPrinted;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<SwitchCases>"), mode);
   }
   UNREACHABLE;
}
//print the cases of a match statement
uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::MatchCases> obj, char mode, FmtArgs fmt) {
   TODO;
}

#pragma endregion lists

//print a type specification
//padForPosSign: print a leading space for non-null nodes
//isLeftJust: print the canon name of the type
uint mcsl::writef(mcsl::File& file, const clef::astTTsB obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }

   const TypeSpec& spec = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      if (fmt.padForPosSign) {
         charsPrinted += file.printf(FMT(" "));
      }
      switch (spec.metaType()) {
         case TypeSpec::null: //unknown type
            if (fmt.isLeftJust) {
               debug_assert(spec.canonName());
               charsPrinted += file.printf(FMT("%s"), TSB(spec.canonName()));
               return charsPrinted;
            }
            TODO;
         case TypeSpec::FUND_TYPE: //fundamental type
            charsPrinted += file.printf(toString(spec.fund().id));
            return charsPrinted;
         case TypeSpec::INDIR: //indirect type (pointer, reference, slice, array)
            charsPrinted += file.printf(FMT("%s%-s%s"), spec.pointeeQuals(), TTsB(spec.pointee()), TItB(&spec.indirTable()));
            return charsPrinted;
         case TypeSpec::COMPOSITE: { //composite object type
            if (fmt.isLeftJust) {
               debug_assert(spec.canonName());
               charsPrinted += file.printf(FMT("%s"), TSB(spec.canonName()));
               return charsPrinted;
            }
            indenter indents = obj.indents + 1;
            bool needsNewline;
            #define __print(field, prefix) \
               needsNewline = false; \
               for (auto symbol : spec.composite().field) { \
                  charsPrinted += file.printf(FMT("%S% s" prefix "%#s;"), indents, symbol.quals, TSB(symbol.symbol)); \
                  needsNewline = true; \
               } \
               if (needsNewline) { charsPrinted += file.printf(FMT("%S"), obj.indents); }
            // __print(tpltParams,);
            __print(impls,);
            __print(subtypes,);
            __print(staticMembs,"static ");
            __print(dataMembs,);
            __print(staticFuncs,"static ");
            __print(methods,);
            #undef __print
            // for (auto [op, symbol] : spec.composite().ops) {
            //    TODO;
            // }
            charsPrinted += charsPrinted;
            return charsPrinted;
         }
         case TypeSpec::FUNC_SIG: //function signature type
            charsPrinted += file.printf(FMT("("));
            if (spec.funcSig().params.size()) {
               charsPrinted += file.printf(FMT("%s% s"), TTsB(spec.funcSig().params[0].first), spec.funcSig().params[0].second);
               for (uint i = 1; i < spec.funcSig().params.size(); ++i) {
                  charsPrinted += file.printf(FMT(", %s% s"), TTsB(spec.funcSig().params[i].first), spec.funcSig().params[i].second);
               }
            }
            if (spec.funcSig().retType) {
               charsPrinted += file.printf(FMT(") -> %s"), TTsB(spec.funcSig().retType));
            } else {
               charsPrinted += file.printf(FMT(") -> auto"));
            }
            return charsPrinted;
         case TypeSpec::FUNC: //function type
            debug_assert(spec.canonName());
            charsPrinted += file.printf(FMT("%s"), TSB(spec.canonName()));
            return charsPrinted;
      }
   } else if ((mode | CASE_BIT) == 'b') {
      TODO;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTTsB"), mode);
   }
   UNREACHABLE;
}
//print a symbol
//padForPosSign: print a leading space for non-null nodes
//altMode: print definition (instead of just the name)
uint mcsl::writef(mcsl::File& file, const clef::astTSB obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }

   const SymbolNode& symbol = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      if (fmt.padForPosSign) {
         charsPrinted += file.printf(FMT(" "));
      }
      if (fmt.altMode) {
         if (symbol.symbolType() == SymbolType::VAR) {
            charsPrinted += file.printf(FMT("%-s "), TTsB(symbol.type()));
            debug_assert(symbol.type());
         }
         else if (symbol.symbolType() == SymbolType::FUNC) {
            for (auto [sig, def] : symbol.overloads()) {
               if (def) {
                  charsPrinted += file.printf(FMT("%s"), TNB_INDENT(def));
               } else {
                  charsPrinted += file.printf(FMT(symbol.name().size() ? "func " : "func"));
                  charsPrinted += file.printf(FMT("%s%s"), symbol.name(), TTsB_INDENT(sig));
               }
            }
            return charsPrinted;
         } else if (isType(symbol.symbolType())) {
            charsPrinted += file.printf(FMT("%s "), toString(symbol.symbolType()));
         }
      }
      charsPrinted += file.printf(FMT("%s"), symbol.name());
      if (symbol.symbolType() == SymbolType::INDIR) {
         debug_assert(symbol.type() && symbol.type()->metaType() == TypeSpec::INDIR);
         charsPrinted += file.printf(FMT("% s%s"), symbol.type()->pointeeQuals(), TItB(&symbol.type()->indirTable()));
      }
      if (fmt.altMode) {
         if (isType(symbol.symbolType())) {
            charsPrinted += file.printf(FMT(" {%s}"), TTsB_INDENT(symbol.type()));
         } else if (symbol.symbolType() == SymbolType::FUNC || symbol.symbolType() == SymbolType::MACRO) {
            TODO;
         }
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      TODO;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTSB"), mode);
   }

   UNREACHABLE;
}
//print an indirection table
//ignores all format codes
uint mcsl::writef(mcsl::File& file, const clef::astTItB& obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj) {
      return 0;
   }

   const IndirTable& table = *obj;
   if ((mode | CASE_BIT) == 's') {
      uint charsPrinted = 0;
      for (uint i = 0; i < table.size(); ++i) {
         IndirTable::Entry entry = table[i];
         IndirTable::ArrExt ext = IndirTable::ArrExt::make();
         if (entry.isArr()) {
            ext = table.arrExtent(i);
         }
         for (uint j = (entry.isArr() && !entry.extIsIndex()) ? 0 : entry.totalRunLength(); j--;) {
            switch (entry.type()) {
               case IndirTable::Entry::PTR:
                  charsPrinted += file.printf(FMT("*"));
                  break;
               case IndirTable::Entry::REF:
                  charsPrinted += file.printf(FMT("&"));
                  break;
               case IndirTable::Entry::SLICE:
                  charsPrinted += file.printf(FMT("[]"));
                  break;
               case IndirTable::Entry::ARR:
                  if (ext.isIndex) {
                     charsPrinted += file.printf(FMT("[%s]"), TNB_AST(ext.i));
                     if (!j) {
                        i += 4;
                     }
                  } else {
                     charsPrinted += file.printf(FMT("[%u]"), ext.size);
                     if (!j) {
                        i += entry.arrSizeBytes();
                     }
                  }
                  break;
            }
            if (entry.isConst()) {
               charsPrinted += file.printf(FMT(" const"));
            }
            if (entry.isVolatile()) {
               charsPrinted += file.printf(FMT(" volatile"));
            }
            if (entry.isAtomic()) {
               charsPrinted += file.printf(FMT(" atomic"));
            }
         }
      }
      return charsPrinted;
   } else if ((mode | CASE_BIT) == 'b') {
      TODO;
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing IndirTable"), mode);
   }
}

#undef TItB_INDENT
#undef TItB

#undef TSB_INDENT
#undef TST

#undef TTsB_INDENT
#undef TTsB

#undef TNB_CAST_INDENT
#undef TNB_CAST2
#undef TNB_CAST
#undef TNB_AST
#undef TNB_INDENT
#undef TNB

//print type qualifiers
//padForPosSign: print *trailing* space for non-null masks
uint mcsl::writef(mcsl::File& file, clef::QualMask quals, char mode, FmtArgs fmt) {
   using namespace clef;
   switch (mode | mcsl::CASE_BIT) {
      case 's': {
         if (!+quals) { return 0; }
         uint charsPrinted = 0;
         for (uint16 bit = 1; +quals && bit; bit <<= 1) {
            QualMask qualbit = quals & (QualMask)bit;
            if (+qualbit) {
               quals &= ~qualbit;
               charsPrinted += file.printf(fmt.padForPosSign || +quals ? FMT("%s ") : FMT("%s"), toString(qualbit));
            }
         }
         return charsPrinted;
      }
      case 'b': {
         return mcsl::writef(file, +quals, mode, fmt);
      }
      default: UNREACHABLE;
   }
}

#pragma GCC diagnostic pop

#endif //CLEF_AST_PRINTF_CPP