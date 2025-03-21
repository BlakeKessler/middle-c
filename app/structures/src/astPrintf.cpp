#ifndef CLEF_AST_PRINTF_CPP
#define CLEF_AST_PRINTF_CPP

#include "SyntaxTree.hpp"
#include "astNode.hpp"
#include "pretty-print.hpp"
#include "io.hpp"

#define MAKE_TNB(expr) clef::astTNB{obj.tree, expr}

uint mcsl::writef(mcsl::File& file, const clef::astTNB<clef::Literal> obj, char mode, FmtArgs fmt) {
   using namespace clef;
   if (!obj.obj) {
      return 0;
   }
   const Literal& lit = obj.tree[obj.obj];
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

         case LitType::TYPEID: return writef(file, astTNB<Type>{obj.tree, (clef::index<const Type>)lit}, mode, fmt);
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

         case LitType::TYPEID: charsPrinted += writef(file, astTNB<Type>{obj.tree, (clef::index<const Type>)lit}, 'b', fmt);
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
   if (!obj.obj) {
      return 0;
   }
   const If& ifstmt = obj.tree[obj.obj];
   if ((mode | CASE_BIT) == 's') { //print as human-readable Middle-C code
      return file.printf(FMT("if (%s) %s else %s;"), MAKE_TNB(ifstmt.condition()), MAKE_TNB(ifstmt.procedure()), MAKE_TNB(ifstmt.elseStmt())); //!TODO: make this handle null indices in a more human-readable way (EX: prevent else statements from printing as `else if () {...} ;`)
   } else if ((mode | CASE_BIT) == 'b') { //print in binary format
      return file.printf(FMT("%b%b%b"), MAKE_TNB(ifstmt.condition()), MAKE_TNB(ifstmt.procedure()), MAKE_TNB(ifstmt.elseStmt()));
   } else {
      __throw(ErrCode::UNSPEC, FMT("unsupported format code (%%%c) for printing astTNB<If>"), mode);
   }
   UNREACHABLE;
}

#pragma endregion exprs

//!TODO: convert all of these into writef implementations
/*
mcsl::File& mcsl::write(File& file, const clef::Expr& expr) {
   using namespace clef;
   file.printf(mcsl::FMT("expression: op=%s("), toString(expr.opID()));
   if (+expr.lhsType()) {
      file.printf(mcsl::FMT("lhs: %s{id=%u}"), toString(expr.lhsType()), +expr.lhs());

      if (+expr.rhsType() || +expr.extraType()) { file.printf(mcsl::FMT(", ")); }
   }
   if (+expr.rhsType()) {
      file.printf(mcsl::FMT("rhs: %s{id=%u}"), toString(expr.rhsType()), +expr.rhs());

      if (+expr.extraType()) { file.printf(mcsl::FMT(", ")); }
   }
   if (+expr.extraType()) {
      file.printf(mcsl::FMT("extra: %s{id=%u}"), toString(expr.extraType()), +expr.extra());
   }
   file.printf(mcsl::FMT(")"));
   return file;
}
mcsl::File& mcsl::write(File& file, const clef::ForeachLoopParams obj) {
   file.printf(mcsl::FMT("FOREACH_PARAMS (id=%u : id=%u)"), +obj.iterator(), +obj.target());
   return file;
}
mcsl::File& mcsl::write(File& file, const clef::ForLoopParams& obj) {
   mcsl::printf(mcsl::FMT("FOR_PARAMS: (id=%u; id=%u; id=%u)"), +obj.decl(), +obj.condition(), +obj.increment());
   return file;
}
mcsl::File& mcsl::write(File& file, const clef::Function& obj) {
   obj.printAs(file, FMT("function"));
   file.printf(FMT(" - (signature id=%u) {procedure id=%u}"), +obj.signature(), +obj.procedure());
   return file;
}
bool clef::Identifier::operator==(const Identifier& other) const {
   if (+_keywordID || +other._keywordID) {
      return _keywordID == other._keywordID;
   }
   return sameName(other) && sameScope(other);
}
void clef::Identifier::__printName(mcsl::File& file) const {
   file.printf(mcsl::FMT("\033[4m"));
   if (+_keywordID) {
      file.printf(mcsl::FMT("%s"), toString(_keywordID));
   } else {
      file.printf(mcsl::FMT("%s"), _name_size, _name_buf);
   }
   file.printf(mcsl::FMT("\033[24m"));
}
void clef::Identifier::printAs(mcsl::File& file, const mcsl::str_slice nodetype) const {
   file.printf(mcsl::FMT("%s: "), nodetype);
   __printName(file);
   file.printf(mcsl::FMT(" (scope: id=%u)"), +scopeName());
}
mcsl::File& mcsl::write(File& file, const clef::Identifier& obj) {
   obj.printAs(file, FMT("identifier"));
   return file;
}
mcsl::File& mcsl::write(mcsl::File& file, const clef::Literal& obj) {
   using namespace clef;
   file.printf(mcsl::FMT("%s literal: "), toString(obj.type()));
   switch (obj.type()) {
      case LitType::NONE: break;

      case LitType::POINTER: file.printf(mcsl::FMT("%r"), (const void*)obj); break;

      case LitType::UINT: file.printf(mcsl::FMT("%u"), (ulong)obj); break;
      case LitType::SINT: file.printf(mcsl::FMT("%i"), (slong)obj); break;
      case LitType::FLOAT: file.printf(mcsl::FMT("%f"), (flong)obj); break;

      case LitType::CHAR: file.printf(mcsl::FMT("%c"), (char)obj); break;
      case LitType::STRING: [[fallthrough]];
      case LitType::INTERP_STR: [[fallthrough]];
      case LitType::FORMAT: [[fallthrough]];
      case LitType::REGEX: file.printf(mcsl::FMT("%s"), (const str_slice&)obj); break;

      case LitType::TYPEID: file.printf(mcsl::FMT("%u"), (clef::index<const Type>)obj); break;
   }
   return file;
}
mcsl::File& mcsl::write(File& file, const clef::Scope& obj) {
   file.printf(FMT("scope:"));
   for (uint i = 0; i < obj.size(); ++i) {
      file.printf(FMT(" id=%u;"), +obj[i]);
   }
   return file;
}
mcsl::File& mcsl::write(File& file, const clef::Type& obj) {
   obj.printAs(file, FMT("type"));
   return file;
}
mcsl::File& mcsl::write(File& file, const clef::Variable& obj) {
   obj.printAs(file, FMT("variable"));
   file.printf(mcsl::FMT(": (type id=%u) = (value id=%u)"), +obj.type(), +obj.val());
   return file;
}
*/

#endif //CLEF_AST_PRINTF_CPP